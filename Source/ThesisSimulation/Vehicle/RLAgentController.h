// copyright (c) 2023 Bas Blokzijl, Leiden University.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RLAgentVehicle.h"
#include "RLAgentController.generated.h"

/**
 * 
 */
UCLASS()
class THESISSIMULATION_API ARLAgentController : public AAIController
{
	GENERATED_BODY()


public:
	ARLAgentController();

	/**
	 * @brief Setup the controller for the given vehicle.
	 * @param NewControlledVehicle Reference to the vehicle that this controller will control.
	 */
	UFUNCTION(Category = "BeginPlay")
	void InitAgentController(ARLAgentVehicle* NewControlledVehicle);

	/** @brief Move the vehicle in random directions */
	void StartMovement();


private:
	
	UPROPERTY()
	ARLAgentVehicle* m_ControlledVehicle;

	// Time between updates of the controlled vehicle movement.
	inline static const float m_UpdateMovementInterval = 0.1f;

	FTimerHandle m_UpdateMovementTimerHdl;
	FTimerDelegate m_UpdateMovementTimerDel;

	/** @brief Moves the controlled vehicle using chaos physics. */
	UFUNCTION()
	void UpdateMovement();
	
};
