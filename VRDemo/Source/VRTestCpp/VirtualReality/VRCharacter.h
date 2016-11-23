// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MotionControllerActor.h"
#include "VRCharacter.generated.h"

UCLASS()
class VRTESTCPP_API AVRCharacter : public ACharacter {
	GENERATED_BODY()

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// 当前玩家操作线指向的NPC。
	/// </summary>
	class ANPCCharacter* TargetedNPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// 当前玩家呼叫交互的NPC。
	/// </summary>
	class ANPCCharacter* InteractedNPC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = true))
	float NearestInteractionDistance = 500.0f;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void GrabLeft();
	void StopGrabLeft();

	void GrabRight();
	void StopGrabRight();

	void PreviewTeleportLeft();
	void CompleteTeleportLeft();

	void PreviewTeleportRight();
	void CompleteTeleportRight();

	void UpdateTeleportRotation(AMotionControllerActor* MotionController);

public:
	// Sets default values for this character's properties
	AVRCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

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

	// ---- Interaction Start ---- //
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OpenTraceLine(const FVector& Start, const FVector& End, const TArray<AActor*>& IgnoreActors);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CallNPC();
	// ---- Interaction End ---- //

	inline AMotionControllerActor* GetLeftMotionController() { return LeftMotionController; }
	inline AMotionControllerActor* GetRightMotionController() { return RightMotionController; }
	
	inline class ANPCCharacter* GetInteractedNPC() const { return InteractedNPC; }
	inline void SetInteractedNPC(class ANPCCharacter* val) { InteractedNPC = val; }
};
