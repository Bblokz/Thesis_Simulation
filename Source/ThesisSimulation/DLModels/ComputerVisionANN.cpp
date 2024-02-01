// copyright (c) 2023 Bas Blokzijl, Leiden University.


#include "ThesisSimulation/DLModels/ComputerVisionANN.h"

UComputerVisionANN::UComputerVisionANN()
{
	UNeuralNetwork* Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
	// Load the ONNX model and set the device (CPU/GPU)
	const FString ONNXModelFilePath = TEXT("SOME_PARENT_FOLDER/SOME_ONNX_FILE_NAME.onnx");
	if (Network->Load(ONNXModelFilePath))
	{
		if (Network->IsGPUSupported())
		{
			Network->SetDeviceType(ENeuralDeviceType::GPU);
		}
		Network->SetDeviceType(ENeuralDeviceType::CPU);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNeuralNetwork could not loaded from %s."), *ONNXModelFilePath);
	}
}
