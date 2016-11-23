// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "AIController.h"

#include "NPCCharacter.h"
#include "VRCharacter.h"


// Sets default values
AVRCharacter::AVRCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	if (RootComponent) {
		VROrigin->SetupAttachment(RootComponent);
	}
	else {
		RootComponent = VROrigin;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay() {
	Super::BeginPlay();

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
		if (DeviceName == "OculusRift" || DeviceName == "SteamVR") {
			UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
		}
		else if (DeviceName == "PSVR") {
			UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
			VROrigin->AddLocalOffset(FVector(0, 0, DefaultPlayerHeight));
			UseControllerRollToRotate = true;
		}

		// Spawn and attach both motion controllers.
		LeftMotionController = SpawnMotionController();
		LeftMotionController->init(EControllerHand::Left);
		RightMotionController = SpawnMotionController();
		RightMotionController->init(EControllerHand::Right);

		FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		LeftMotionController->AttachToComponent(VROrigin, rules);
		RightMotionController->AttachToComponent(VROrigin, rules);
	}
	else {
		VROrigin->AddLocalOffset((FVector(0, 0, DefaultPlayerHeight)));
	}
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		// 左手控制器。
		UpdateTeleportRotation(LeftMotionController);
		// 右手控制器。
		UpdateTeleportRotation(RightMotionController);
	}
	else {
		// 无头戴显示器时，Camera下开启轨迹射线。
		FVector Start = Camera->GetComponentLocation() + FVector(0, 0, -10);
		FVector End = Start + (Camera->GetComponentRotation().Vector() * 10000.0f);
		OpenTraceLine(Start, End, { this });
	}
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(class UInputComponent* InputComp) {
	Super::SetupPlayerInputComponent(InputComp);

	// Setup gameplay key binddings.
	check(InputComp);

	InputComp->BindAction("GrabLeft", IE_Pressed, this, &AVRCharacter::GrabLeft);
	InputComp->BindAction("GrabLeft", IE_Released, this, &AVRCharacter::StopGrabLeft);
	InputComp->BindAction("GrabRight", IE_Pressed, this, &AVRCharacter::GrabRight);
	InputComp->BindAction("GrabRight", IE_Released, this, &AVRCharacter::StopGrabRight);

	InputComp->BindAction("TeleportLeft", IE_Pressed, this, &AVRCharacter::PreviewTeleportLeft);
	InputComp->BindAction("TeleportLeft", IE_Released, this, &AVRCharacter::CompleteTeleportLeft);
	InputComp->BindAction("TeleportRight", IE_Pressed, this, &AVRCharacter::PreviewTeleportRight);
	InputComp->BindAction("TeleportRight", IE_Released, this, &AVRCharacter::CompleteTeleportRight);

	InputComp->BindAxis("MotionControllerThumbLeft_Y");
	InputComp->BindAxis("MotionControllerThumbLeft_X");
	InputComp->BindAxis("MotionControllerThumbRight_Y");
	InputComp->BindAxis("MotionControllerThumbRight_X");


	InputComp->BindAxis("MoveForward", this, &AVRCharacter::MoveForward);
	InputComp->BindAxis("MoveRight", this, &AVRCharacter::MoveRight);
	InputComp->BindAxis("LookUp", this, &AVRCharacter::LookUp);
	InputComp->BindAxis("Turn", this, &AVRCharacter::Turn);
	InputComp->BindAction("InteractWithAI", IE_Pressed, this, &AVRCharacter::CallNPC);
}

AMotionControllerActor* AVRCharacter::SpawnMotionController() {
	UWorld* const World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		return World->SpawnActor<AMotionControllerActor>(MotionControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	return nullptr;
}

void AVRCharacter::MoveForward(float Value) {
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, Value);
}

void AVRCharacter::MoveRight(float Value) {
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	FVector Direction = GetActorRightVector();
	AddMovementInput(Direction, Value);
}

void AVRCharacter::LookUp(float Value) {
	FRotator NewRotation = Camera->GetRelativeTransform().Rotator();
	NewRotation.Pitch += Value;
	Camera->SetRelativeRotation(NewRotation);
}
void AVRCharacter::Turn(float Value) {
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Value;
	Controller->SetControlRotation(NewRotation);
	//SetActorRotation(NewRotation);
}

void AVRCharacter::GrabLeft() {
	LeftMotionController->GrabActor();
}

void AVRCharacter::StopGrabLeft() {
	LeftMotionController->ReleaseActor();
}

void AVRCharacter::GrabRight() {
	RightMotionController->GrabActor();

	CallNPC();
}

void AVRCharacter::StopGrabRight() {
	RightMotionController->ReleaseActor();
}

void AVRCharacter::PreviewTeleportLeft() {
	LeftMotionController->ActivateTeleporter();
	RightMotionController->DisableTeleporter();
}

void AVRCharacter::CompleteTeleportLeft() {
	if (LeftMotionController->GetIsTeleporterActive()) {
		ExcuteTeleportation(LeftMotionController);
	}
}

void AVRCharacter::PreviewTeleportRight() {
	RightMotionController->ActivateTeleporter();
	LeftMotionController->DisableTeleporter();
}

void AVRCharacter::CompleteTeleportRight() {
	if (RightMotionController->GetIsTeleporterActive()) {
		ExcuteTeleportation(RightMotionController);
	}
}

void AVRCharacter::UpdateTeleportRotation(AMotionControllerActor* MotionController) {
	if (MotionController->GetIsTeleporterActive()) {
		float Axis_Y;
		float Axis_X;
		if (MotionController->Hand == EControllerHand::Left) {
			Axis_Y = GetInputAxisValue("MotionControllerThumbLeft_Y");
			Axis_X = GetInputAxisValue("MotionControllerThumbLeft_X");
		}
		else {
			Axis_Y = GetInputAxisValue("MotionControllerThumbRight_Y");
			Axis_X = GetInputAxisValue("MotionControllerThumbRight_X");
		}
		FRotator TeleportRotation = GetRotationFromInput(Axis_Y, Axis_X, MotionController);
		MotionController->SetTeleportRotation(TeleportRotation);
	}
}

void AVRCharacter::ExcuteTeleportation(AMotionControllerActor* MotionController) {
	if (!IsTeleporting) {
		if (MotionController->GetIsValidTeleportDestination()) {
			IsTeleporting = true;
			// 摄像机淡出。
			APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
			CameraManager->StartCameraFade(0.0f, 1.0f, FadeOutDuration, TeleportFadeColor, false, true);
			//// 设置延时关闭传送。
			//FLatentActionInfo LatentInfo;
			//LatentInfo.CallbackTarget = MotionController;
			//LatentInfo.UUID = FMath::FRand() * 100000;
			//LatentInfo.ExecutionFunction = FName(TEXT("DisableTeleporter"));
			//LatentInfo.Linkage = 0;
			//UKismetSystemLibrary::Delay(this, 0.1f, LatentInfo);
			MotionController->DisableTeleporter();
			// 执行传送。
			FVector TeleportLocation;
			FRotator TeleportRotation;
			MotionController->GetTeleportDestination(TeleportLocation, TeleportRotation);
			TeleportTo(TeleportLocation, TeleportRotation);
			// 摄像机淡入。
			CameraManager->StartCameraFade(1.0f, 0.0f, FadeInDuration, TeleportFadeColor);
			IsTeleporting = false;
		}
		else {
			MotionController->DisableTeleporter();
		}
	}
}

FRotator AVRCharacter::GetRotationFromInput(float UpAxis, float RightAxis, AMotionControllerActor* MotionController) {
	if (UseControllerRollToRotate) {
		// 使用控制器自身旋转来获取朝向。PSVR强制使用此方式（PS Move Motion Controller没有类摇杆输入）。
		FTransform InitialTransform = UKismetMathLibrary::MakeTransform(FVector::ZeroVector, MotionController->GetInitialControllerRotation(), FVector::ZeroVector);
		FTransform CurrentTransform = MotionController->GetTransform();
		FRotator RelativeRotation = UKismetMathLibrary::ConvertTransformToRelative(InitialTransform, CurrentTransform).Rotator();
		// 将控制器Roll值转换为玩家朝向Yaw，放大3倍变化。
		float Yaw = GetActorRotation().Yaw + RelativeRotation.Roll * 3.0f;
		return FRotator(0, Yaw, 0);
	}
	else {
		// 使用控制器类摇杆输入的XY值来获取朝向。
		if (UKismetMathLibrary::Abs(UpAxis) + UKismetMathLibrary::Abs(RightAxis) >= ThumbDeadzone) {
			// Controller Up Axis -> X, Controller Right Axis -> Y。
			FVector XYOrientation = FVector(UpAxis, RightAxis, 0).GetSafeNormal();
			// 控制器输入的“前方”总是相对于Pawn的旋转来选择。
			XYOrientation = FRotator(0, GetActorRotation().Yaw, 0).RotateVector(XYOrientation);
			return UKismetMathLibrary::Conv_VectorToRotator(XYOrientation);
		}
		else {
			// 类摇杆中间一定范围位置按下则忽略XY，不改变朝向。
			return GetActorRotation();
		}
	}
}

void AVRCharacter::OpenTraceLine(const FVector& Start, const FVector& End, const TArray<AActor*>& IgnoreActors) {
	if (TargetedNPC) {
		TargetedNPC->SetInteractionTargeted(false);
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Pawn, Params);
	if (bHit) {
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Yellow, "HitTarget: " + Hit.Actor.Get()->GetName());
		UKismetSystemLibrary::DrawDebugSphere(this, Hit.ImpactPoint, 2.5f, 12, FColor::Emerald, 0, 2);
		UKismetSystemLibrary::DrawDebugLine(this, Hit.TraceStart, Hit.ImpactPoint, FColor::Emerald, 0, 1);
		TargetedNPC = Cast<ANPCCharacter>(Hit.Actor.Get());
		if (TargetedNPC) {
			TargetedNPC->SetInteractionTargeted(true);
		}
	}
	else {
		UKismetSystemLibrary::DrawDebugLine(this, Hit.TraceStart, Hit.TraceEnd, FColor::Emerald, 0, 1);
		InteractedNPC = TargetedNPC = nullptr;
	}
}

void AVRCharacter::CallNPC() {
	if (TargetedNPC) {
		if (InteractedNPC && InteractedNPC != TargetedNPC) {
			InteractedNPC->SetInteractionState(EInteractionState::Dismissed);
		}

		InteractedNPC = TargetedNPC;
		InteractedNPC->SetInteractionState(EInteractionState::Called);

		FVector DestLocation;
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
			// HMD相对Camera位移信息。
			FRotator HMDRotation;
			FVector HMDPosition;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDPosition);
			FVector HMDXYPosition = FVector(HMDPosition.X, HMDPosition.Y, 0);
			FRotator HMDYawRotation = FRotator(0, HMDRotation.Yaw, 0);
			// 考虑Actor本身的旋转与HMD的位移旋转，计算当前HMD朝向前方指定距离的点。
			DestLocation = GetActorLocation() + GetActorRotation().RotateVector(HMDXYPosition) + UKismetMathLibrary::ComposeRotators(GetActorRotation(), HMDYawRotation).Vector() * InteractedNPC->GetInteractionDistance();
		}
		else {
			DestLocation = GetActorLocation() + GetActorRotation().Vector() * InteractedNPC->GetInteractionDistance();
		}

		InteractedNPC->DestLocation = DestLocation;
	}
}
