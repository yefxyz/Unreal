// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PickupActorInterface.h"
#include "PickupCube.generated.h"

/**
 *
 */
UCLASS()
class VRTESTCPP_API APickupCube : public AStaticMeshActor, public IPickupActorInterface {
	GENERATED_BODY()

public:
	APickupCube();

	void Pickup_Implementation(USceneComponent* Picker);

	void Drop_Implementation();

};
