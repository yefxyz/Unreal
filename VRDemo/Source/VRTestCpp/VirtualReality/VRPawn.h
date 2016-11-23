// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MotionControllerActor.h"
#include "VRPawn.generated.h"

UCLASS()
class VRTESTCPP_API AVRPawn : public APawn {
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USceneComponent* VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	AMotionControllerActor* LeftMotionController;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	AMotionControllerActor* RightMotionController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	TSubclassOf<AMotionControllerActor> MotionControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	float FadeInDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	float FadeOutDuration = 0.2f;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	bool IsTeleporting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	FColor TeleportFadeColor = FColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	float ThumbDeadzone = 0.7f;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	bool LeftStickDown;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	bool RightStickDown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	float DefaultPlayerHeight = 180.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Chaperone", meta = (AllowPrivateAccess = true))
	bool UseControllerRollToRotate;

	AMotionControllerActor* SpawnMotionController();
	class UVRPawnMovementComponent* VRPawnMovementComponent;
	class UWidgetInteractionComponent* WidgetInteractionComponent;

protected:

	void GrabLeft();
	void StopGrabLeft();

	void GrabRight();
	void StopGrabRight();

	void PreviewTeleportLeft();
	void CompleteTeleportLeft();

	void PreviewTeleportRight();
	void CompleteTeleportRight();

	void UpdateTeleportRotation(AMotionControllerActor* MotionController);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void SearchTarget();
public:
	// Sets default values for this pawn's properties
	AVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 执行传送。
	/// </summary>
	/// <param name="MotionController">The motion controller.</param>
	void ExcuteTeleportation(AMotionControllerActor* MotionController);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 由控制器输入获取旋转方向。
	/// </summary>
	/// <param name="UpAxis">Up axis.</param>
	/// <param name="RightAxis">The right axis.</param>
	/// <param name="MotionController">The motion controller.</param>
	/// <returns>FRotator.</returns>
	FRotator GetRotationFromInput(float UpAxis, float RightAxis, AMotionControllerActor* MotionController);

	virtual UPawnMovementComponent* GetMovementComponent() const override;
};
