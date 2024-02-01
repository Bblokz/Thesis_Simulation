// copyright (c) 2023 Bas Blokzijl, Leiden University.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Camera/CameraComponent.h"
#include "Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Engine/Classes/Engine/TextureRenderTarget2D.h"
#include <string>
#include <fstream>

#include "ThesisSimulation/Vehicle/RLAgentVehicle.h"
#include "CustomScreenCapture.generated.h"

USTRUCT()
struct FVehicleData
{
	GENERATED_BODY()
	float Throttle;
	float Steering;
	float Brake;
	float Speed;

	FVehicleData()
	{
		Throttle = 0.0f;
		Steering = 0.0f;
		Brake = 0.0f;
		Speed = 0.0f;
		
	}
	FVehicleData(float NewThrottle, float NewSteering, float NewBrake, float NewSpeed)
	{
		Throttle = NewThrottle;
		Steering = NewSteering;
		Brake = NewBrake;
		Speed = NewSpeed;
	}
	
};

UCLASS()
class THESISSIMULATION_API ACustomScreenCapture : public AActor
{
	GENERATED_BODY()

public:
	ACustomScreenCapture();
	
	virtual void Tick(float DeltaTime) override;

	/** @brief The image resolution on x-axis as specified per script blueprint. */
	UPROPERTY(EditAnywhere, Category = "Output Information", meta = (ClampMin = "32", ClampMax = "4096", UIMin = "32", UIMax = "4096"))
	uint32 ResolutionX;

	/** @brief The image resolution on y-axis as specified per script blueprint. */
	UPROPERTY(EditAnywhere, Category = "Output Information", meta = (ClampMin = "32", ClampMax = "4096", UIMin = "32", UIMax = "4096"))
	uint32 ResolutionY;

	UPROPERTY(EditAnywhere, Category = "Output Information", meta = (ClampMin = "20.0", ClampMax = "179.9", UIMin = "20.0", UIMax = "179.9"))
	float FieldOfView;

	UPROPERTY(EditAnywhere, Category = "Output Information")
	FString CVOutputFolderPath;

	UFUNCTION(BlueprintCallable)
	void StartDataGathering();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Find References")
	ARLAgentVehicle* GetVehicleReference();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	// Create the image and save it to disk.
	void GatherImageAndVehicleData();

	void RunPythonScript();

	// Textures need to be power of 2; texture has to be square.
	uint32_t m_InternResolution;

	/** @brief The target to which the camera will render a scene capture.  */
	UPROPERTY()
	UTextureRenderTarget2D* m_RenderTarget;
	
	/** @brief The capture component that sets post processing settings. */
	UPROPERTY()
	class USceneCaptureComponent2D* m_SceneCapture;

	/** @brief The camera component that sets the view of the scene capture. */
	UPROPERTY()
	class UCameraComponent* m_Camera;
	
	uint32_t m_TickCount;
	std::string m_BaseFilename;
	std::string m_BasePathFolder;

	// Reference to the vehicle.
	UPROPERTY()
	ARLAgentVehicle* m_Vehicle;



	/**
	 * @brief Writes the captured buffer in rgba format to the specified folder.
	 * @param Buffer8 The captured buffer of a frame using the FColor UE format.
	 * @note Uses one byte per channel.
	 */
	void WriteRawBytesToBasePathFolder(TArray<FColor> Buffer8);

	/**
	 * @brief Deletes all images in the specified folder with the specified extension.
	 * @param PlatformFile To access the folders and files.
	 * @param OutFolderPath The path to the folder to delete the images from.
	 * @param FileExtension The type of file to delete.
	 */
	void DeleteImagesInFolder(IPlatformFile& PlatformFile, FString OutFolderPath, FString FileExtension);

	/**
	 * @brief Uses the vehicle reference to obtain input data on the movement component.
	 * @return The input data of the vehicle.
	 */
	FORCEINLINE FVehicleData GetVehicleData();

	void SaveVehicleData(const FVehicleData& vehicleData);

	// Filestream in which the vehicle data is written.
	std::ofstream VehicleDataFile;

	// If set to true, then we are able to obtain input from the vehicle.
	bool m_StartGatheringData;

	// The steering of the vehicle in the previous frame.
	float m_LastSteerValue = 0.f;

};
