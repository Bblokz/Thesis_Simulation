// copyright (c) 2023 Bas Blokzijl, Leiden University.
#include "RLAgentVehicle.h"

#include "RLAgentController.h"

FName ARLAgentVehicle::VehicleMovementComponentName(TEXT("ChaosVehicleMovementComp"));
FName ARLAgentVehicle::VehicleMeshComponentName(TEXT("VehicleMesh"));




ARLAgentVehicle::ARLAgentVehicle()
{
	ChaosVehicleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(VehicleMeshComponentName);
	ChaosVehicleMesh->BodyInstance.bSimulatePhysics = true;
	ChaosVehicleMesh->BodyInstance.bNotifyRigidBodyCollision = true;
	ChaosVehicleMesh->BodyInstance.bUseCCD = true;
	ChaosVehicleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChaosVehicleMesh->bBlendPhysics = true;
	ChaosVehicleMesh->SetGenerateOverlapEvents(true);
	ChaosVehicleMesh->SetCanEverAffectNavigation(false);
	RootComponent = ChaosVehicleMesh;

	ChaosVehicleMovementComponent = CreateDefaultSubobject<URlAgentChaosMovementComponent>(VehicleMovementComponentName);
	ChaosVehicleMovementComponent->UpdatedComponent = ChaosVehicleMesh;
	ChaosVehicleMovementComponent->SetUseAutomaticGears(true);
	
}


void ARLAgentVehicle::BeginPlay()
{
	Super::BeginPlay();
	SetupController();
}

void ARLAgentVehicle::SetupController()
{
	ARLAgentController* AIController = Cast<ARLAgentController>(GetController());

	if (!AIController)
	{
		UWorld* World = GetWorld();
		if(World)
		{
			AIController = World->SpawnActor<ARLAgentController>();
			AIController->Possess(this);
		}
	}
	AIController->InitAgentController(this);
	AIController->StartMovement();
}



