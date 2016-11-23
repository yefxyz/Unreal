// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "Kismet/KismetMathLibrary.h"

#include "NPCCharacter.h"
#include "CharacterWidgetComponent.h"


UCharacterWidgetComponent::UCharacterWidgetComponent() {
	// TODO
}

void UCharacterWidgetComponent::PostLoad() {
	Super::PostLoad();

	SetRelativeLocation(FVector(0, Radius, Height));
	SetRelativeScale3D(FVector(Scale, Scale, Scale));
}

void UCharacterWidgetComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();

	// 显示范围判断。
	ANPCCharacter* NPCOwner = Cast<ANPCCharacter>(Owner);
	if (NPCOwner) {
		SetVisibility(bShowWidget && NPCOwner->IsShowWidget());
	}

	// Widget随Camera旋转，以Character为轴，始终面向Camera。
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager) {
		FRotator CameraRotation = CameraManager->GetCameraRotation();
		FRotator WidgetRotation = FRotator(0, CameraRotation.Yaw - Owner->GetActorRotation().Yaw + 180.0f, 0);
		SetRelativeRotation(WidgetRotation);

		FVector WidgetOrigin = FVector(0, Radius, Height);
		SetRelativeLocation(WidgetRotation.RotateVector(WidgetOrigin));
	}
}
