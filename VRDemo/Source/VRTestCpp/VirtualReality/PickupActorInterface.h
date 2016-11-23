// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PickupActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupActorInterface : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class VRTESTCPP_API IPickupActorInterface {
	GENERATED_IINTERFACE_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void Pickup(USceneComponent* Picker);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void Drop();
};
