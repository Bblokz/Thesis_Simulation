// copyright (c) 2023 Bas Blokzijl, Leiden University.


#include "ThesisSimulation/PlayerCon/RLPlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"
#include "ThesisSimulation/Vehicle/RLAgentVehicle.h"


ARLPlayerController::ARLPlayerController()
{
	// Create the camera boom
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->TargetArmLength = 400.0f; // Start with 400 cm distance
	m_CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f)); // 45 degree angle

	// Create the camera and attach it to the camera boom
	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Camera->SetupAttachment(m_CameraBoom);
}



void ARLPlayerController::ZoomIn()
{
	m_CameraBoom->TargetArmLength = FMath::Clamp(m_CameraBoom->TargetArmLength - 10.0f, m_MinCameraDistance, m_MaxCameraDistance);
}

void ARLPlayerController::ZoomOut()
{
	m_CameraBoom->TargetArmLength = FMath::Clamp(m_CameraBoom->TargetArmLength + 10.0f, m_MinCameraDistance, m_MaxCameraDistance);
}
