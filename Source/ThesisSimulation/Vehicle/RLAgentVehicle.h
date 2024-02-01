// copyright (c) 2023 Bas Blokzijl, Leiden University.

#pragma once

#include "CoreMinimal.h"
#include "ThesisSimulation/Vehicle/Components/RlAgentChaosMovementComponent.h"
#include "RLAgentVehicle.generated.h"

/**
 * 
 */
UCLASS()
class THESISSIMULATION_API ARLAgentVehicle : public APawn
{
	GENERATED_BODY()

public:
	ARLAgentVehicle();
	
	static FName VehicleMeshComponentName;
	static FName VehicleMovementComponentName;
	static FName CameraComponentName;
	
	/** @return Vehicle movement component */
	UFUNCTION(BlueprintCallable)
	inline URlAgentChaosMovementComponent* GetVehicleMovementComponent()const {return ChaosVehicleMovementComponent;};

	/** @return Mesh sub-object **/
	inline USkeletalMeshComponent* GetMesh() const { return ChaosVehicleMesh; }

protected:
	
	/**  The main skeletal mesh associated with this Vehicle */
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChaosVehicleMesh;

	/** vehicle simulation component */
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URlAgentChaosMovementComponent* ChaosVehicleMovementComponent;

	// In ARLAgentVehicle.h
	virtual void BeginPlay() override;

private:
	/** @brief Initiates the AI controller with this vehicle as the controlled pawn. Starts movement */
	void SetupController();

	

	

};
