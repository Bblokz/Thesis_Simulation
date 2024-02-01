// copyright (c) 2023 Bas Blokzijl, Leiden University.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RLPlayerController.generated.h"

// forward declarations
class THESISSIMULATION_API ARLAgentVehicle;
class ENGINE_API USpringArmComponent;
class ENGINE_API UCameraComponent;

/**
 * 
 */
UCLASS()
class THESISSIMULATION_API ARLPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARLPlayerController();


protected:

	/** Handle camera zoom based on mouse wheel input */
	UFUNCTION(BlueprintCallable, Category = "MoveCamera")
	void ZoomIn();

	UFUNCTION(BlueprintCallable, Category = "MoveCamera")
	void ZoomOut();

private:
	/** Reference to the controlled RLAgentVehicle */
	UPROPERTY()
	ARLAgentVehicle* m_RLAgentVehicle;

	/** Camera boom positioning the camera behind the agent */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* m_CameraBoom;

	/** The camera itself */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_Camera;

	/** Camera distance constraints */
	const float m_MinCameraDistance = 200.0f;
	const float m_MaxCameraDistance = 600.0f;




};
