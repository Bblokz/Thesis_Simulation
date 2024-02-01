// copyright (c) 2023 Bas Blokzijl, Leiden University.


#include "CustomScreenCapture.h"
#include <algorithm>    // max()
#include <fstream>
// To save images with openCV.
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
// For converting the images.
#include "opencv2/core/mat.hpp"

// To remove the images generated in the previous run.
#include <opencv2/imgproc.hpp>

#include "HAL/PlatformFilemanager.h"




// Display for 1.5 seconds.
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text) 

// Display for x seconds.
#define printFString(text, fstring, floatDisplayTime) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, float(floatDisplayTime), FColor::Magenta, FString::Printf(TEXT(text), fstring))  

#define PRINT_DEBUG_MESSAGES 0


inline const uint32_t SmallestPowerOf2_GE_N(uint32_t N)
{
	N--;
	N |= N >> 1;
	N |= N >> 2;
	N |= N >> 4;
	N |= N >> 8;
	N |= N >> 16;
	N++;
	return N;
}

// Sets default values.
ACustomScreenCapture::ACustomScreenCapture()
	: ResolutionX(256)
	  , ResolutionY(256)
	  , FieldOfView(90.0f)
	, CVOutputFolderPath(TEXT("D:/Thesis/ThesisSimulation/OutputImages"))
	  , m_InternResolution(0)
	  , m_TickCount(0)
	  , m_BaseFilename("")
	  , m_BasePathFolder("")

{
	PrimaryActorTick.bCanEverTick = true;
	this->DisableComponentsSimulatePhysics();

	// Perform tick after all updates, movement, physics is complete.
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	// Setup components.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewportCamera"));
	m_Camera->SetupAttachment(RootComponent);
	m_SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture")); // *****
	m_SceneCapture->SetupAttachment(m_Camera);

	m_BaseFilename = "ScreenTest";
}

void ACustomScreenCapture::StartDataGathering()
{
	m_StartGatheringData = true;
}

void ACustomScreenCapture::BeginPlay()
{
	Super::BeginPlay();
	m_BasePathFolder = std::string(TCHAR_TO_UTF8(*CVOutputFolderPath));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory(*CVOutputFolderPath);
	if (m_BasePathFolder.back() != '/')
		m_BasePathFolder.append("/");

	// Resolution has to be a power of 2. 
	// Finds the lowest RxR resolution of equal size or larger.
	uint32_t Higher = std::max(ResolutionX, ResolutionY);
	m_InternResolution = SmallestPowerOf2_GE_N(Higher);
	if(PRINT_DEBUG_MESSAGES)
	{
		printFString("internResolution is: %d", m_InternResolution, 5.f);	
	}
	m_Camera->FieldOfView = FieldOfView;
	m_SceneCapture->FOVAngle = FieldOfView;
	m_RenderTarget = NewObject<UTextureRenderTarget2D>();
	m_RenderTarget->InitCustomFormat(m_InternResolution, m_InternResolution, EPixelFormat::PF_B8G8R8A8, true);
	// some testing with EPixelFormat::PF_FloatRGBA, true=force Linear Gamma
	m_RenderTarget->TargetGamma = 2.0f;
	m_RenderTarget->UpdateResourceImmediate();

	// SCS_FinalColorLDR allows for post processing on the image.  default = SCS_SceneColorHDR.
	m_SceneCapture->CaptureSource = SCS_FinalColorLDR;
	m_SceneCapture->TextureTarget = m_RenderTarget;
	m_SceneCapture->bCaptureEveryFrame = true;
	// This allows velocities for Motion Blur and Temporal AA to be computed.
	m_SceneCapture->bAlwaysPersistRenderingState = true;
	m_SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;
	// lost over 3 hours here...
	m_SceneCapture->bEnableClipPlane = false;
	
	m_TickCount = 0;

	// remove the images generated in the previous run.
	DeleteImagesInFolder(PlatformFile, CVOutputFolderPath, TEXT(".png"));
	DeleteImagesInFolder(PlatformFile, CVOutputFolderPath, TEXT(".csv"));

	// Obtain a reference to the vehicle.
	m_Vehicle = GetVehicleReference();

	// Open the vehicle data file here.
	std::string BaseCVPath = TCHAR_TO_UTF8(*CVOutputFolderPath);
	std::string CSVFilename = BaseCVPath + "/vehicle_data.csv";
	VehicleDataFile.open(CSVFilename);
	// Writing the headers to the CSV file.
	if (VehicleDataFile.is_open())
	{
		VehicleDataFile << "Count,Throttle,Steering, LastSteering, Speed\n";
	}
	
}

ARLAgentVehicle* ACustomScreenCapture::GetVehicleReference_Implementation()
{
	return nullptr;
}


void ACustomScreenCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_TickCount++;
	if(m_StartGatheringData)
	{
		GatherImageAndVehicleData();
	}
}

void ACustomScreenCapture::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (VehicleDataFile.is_open())
	{
		VehicleDataFile.close();
	}
}

void ACustomScreenCapture::GatherImageAndVehicleData()
{
	auto RenderTargetResource = m_RenderTarget->GameThread_GetRenderTargetResource();

	if (RenderTargetResource && m_Vehicle)
	{
		TArray<FColor> Buffer8;
		RenderTargetResource->ReadPixels(Buffer8);

		if(PRINT_DEBUG_MESSAGES)
		{
			printFString("if (RenderTargetResource) tickCount: %d", m_TickCount, 0.05f);	
		}
		
		cv::Mat WrappedImage(m_RenderTarget->GetSurfaceHeight(), m_RenderTarget->GetSurfaceWidth(), CV_8UC4, Buffer8.GetData());

		// Convert from BGRA to BGR.
		cv::Mat WrappedImage_BGR;
		cv::cvtColor(WrappedImage, WrappedImage_BGR, cv::COLOR_BGRA2BGR);
		
		std::string BaseCVPath = TCHAR_TO_UTF8(*CVOutputFolderPath);
		std::string PNGFilename = BaseCVPath + "/image_number_" + std::to_string(m_TickCount) + ".png";
		
		if (cv::imwrite(PNGFilename, WrappedImage_BGR))
		{
			if (PRINT_DEBUG_MESSAGES)
			{
				printFString("Image saved successfully", m_TickCount, 0.05f);
			}
			
			FVehicleData vehicleData = GetVehicleData();
			if(vehicleData.Speed > 0.0f)
			{
				SaveVehicleData(vehicleData);
			}
			
		}
		else
		{
			if (PRINT_DEBUG_MESSAGES)
			{
				printFString("Failed to save image", m_TickCount, 0.05f);
			}
		}
	}
}

void ACustomScreenCapture::SaveVehicleData(const FVehicleData& vehicleData)
{
	if (VehicleDataFile.is_open())
	{
		// Write vehicle data to CSV.
		VehicleDataFile << m_TickCount << "," << vehicleData.Throttle << "," << vehicleData.Steering << "," << m_LastSteerValue << "," << vehicleData.Speed << "\n";
		m_LastSteerValue = vehicleData.Steering;
		if (PRINT_DEBUG_MESSAGES)
		{
			printFString("Vehicle data saved successfully", m_TickCount, 0.05f);
		}
	}
	else
	{
		if (PRINT_DEBUG_MESSAGES)
		{
			printFString("Failed to save vehicle data", m_TickCount, 0.05f);
		}
	}
}


void ACustomScreenCapture::RunPythonScript() {
	// command to activate the virtual environment and run the Python script
	std::string command = "cmd /c D:\\Thesis\\ThesisSimulation\\Python\\DPT\\venv\\Scripts\\activate";
	
	int result = system(command.c_str());
	if(result == 0) {
		// Success
		if(PRINT_DEBUG_MESSAGES) {
			printFString("Python script ran successfully", m_TickCount, 0.05f);
		}
	}
	else {
		// Error
		if(PRINT_DEBUG_MESSAGES) {
			printFString("Failed to run python script", m_TickCount, 0.05f);
		}
	}
}

void ACustomScreenCapture::WriteRawBytesToBasePathFolder(TArray<FColor> Buffer8)
{
	m_BaseFilename = m_BasePathFolder + std::string("image");
	m_BaseFilename += std::string("_number_");
	std::string FileName = m_BaseFilename + std::to_string(m_TickCount) + std::string(".bgra8");
	std::ofstream TargetFile(FileName, std::ofstream::binary);
	TargetFile.write(reinterpret_cast<char*>(Buffer8.GetData()), Buffer8.Num() * sizeof(FColor));
	TargetFile.close();
}

void ACustomScreenCapture::DeleteImagesInFolder(
	IPlatformFile& PlatformFile,
	FString OutFolderPath,
	FString FileExtension)
{
	if (PlatformFile.DirectoryExists(*OutFolderPath))
	{
		TArray<FString> FilesToDelete;
		PlatformFile.FindFiles(FilesToDelete, *OutFolderPath , *FileExtension);

		for (FString& File : FilesToDelete)
		{
            
			// Ensure the file is writable
			PlatformFile.SetReadOnly(*File, false);
            
			// Try to delete the file
			if(PlatformFile.DeleteFile(*File))
			{
				print("Deleted file!");
			}
		}
	}
}

FVehicleData ACustomScreenCapture::GetVehicleData()
{
	return FVehicleData(
	m_Vehicle->GetVehicleMovementComponent()->GetThrottleInput(),
	m_Vehicle->GetVehicleMovementComponent()->GetSteeringInput(),
	m_Vehicle->GetVehicleMovementComponent()->GetBrakeInput(),
	m_Vehicle->GetVelocity().Length()
	);
}


//  std::string OutputFile(TCHAR_TO_UTF8(*OutputVideoFile));
// cv::imwrite(OutputFile, wrappedImage);