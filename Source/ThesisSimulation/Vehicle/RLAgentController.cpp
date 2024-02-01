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
}
