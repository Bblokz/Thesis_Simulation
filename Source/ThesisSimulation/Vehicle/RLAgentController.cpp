// copyright (c) 2023 Bas Blokzijl, Leiden University.


#include "RLAgentController.h"

ARLAgentController::ARLAgentController()
{
}

void ARLAgentController::InitAgentController(ARLAgentVehicle* NewControlledVehicle)
{
	m_ControlledVehicle = NewControlledVehicle;
}

void ARLAgentController::StartMovement()
{
	UWorld* World = GetWorld();
	if (World)
	{
		m_UpdateMovementTimerDel.BindUObject(this, &ARLAgentController::UpdateMovement);
		World->GetTimerManager().SetTimer(m_UpdateMovementTimerHdl, m_UpdateMovementTimerDel,
		                                  m_UpdateMovementInterval, true);
	}
}

void ARLAgentController::UpdateMovement()
{
	if (m_ControlledVehicle)
	{
		URlAgentChaosMovementComponent* MovementComponent = m_ControlledVehicle->GetVehicleMovementComponent();

		if (MovementComponent)
		{
			//float RandomThrottle = FMath::RandRange(0.0f, 1.0f);
			// float RandomThrottle = 1.0f;
			// float RandomSteering  = 0.f; // FMath::RandRange(0.0f, 1.0f); 
			// MovementComponent->SetThrottleInput(RandomThrottle);
			// MovementComponent->SetSteeringInput(RandomSteering);
			// MovementComponent->SetTargetGear(1, true);
		}
	} 
}
