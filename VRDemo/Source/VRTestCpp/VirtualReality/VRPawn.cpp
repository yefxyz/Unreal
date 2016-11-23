// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "VRPawnMovementComponent.h"
#include "./TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "VRPawn.h"


// Sets default values
AVRPawn::AVRPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	VRPawnMovementComponent = CreateDefaultSubobject<UVRPawnMovementComponent>("MovementCompnoent");
	VRPawnMovementComponent->SetUpdatedComponent(RootComponent);

}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay() {
	Super::BeginPlay();

	FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	if (DeviceName == "OculusRift" || DeviceName == "SteamVR") {
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else if (DeviceName == "PSVR") {
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		VROrigin->AddLocalOffset(FVector(0, 0, DefaultPlayerHeight));
		UseControllerRollToRotate = true;
	}
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		VROrigin->AddLocalOffset((FVector(0, 0, DefaultPlayerHeight)));
	}
	// Spawn and attach both motion controllers.
	LeftMotionController = SpawnMotionController();
	LeftMotionController->init(EControllerHand::Left);
	RightMotionController = SpawnMotionController();
	RightMotionController->init(EControllerHand::Right);

	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	LeftMotionController->AttachToComponent(VROrigin, rules);
	RightMotionController->AttachToComponent(VROrigin, rules);

	//WidgetInteractionComponent = FindComponentByClass<UWidgetInteractionComponent>();
	//WidgetInteractionComponent->bShowDebug = true;
}

// Called every frame
void AVRPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// 左手控制器。
	UpdateTeleportRotation(LeftMotionController);
	// 右手控制器。
	UpdateTeleportRotation(RightMotionController);

	SearchTarget();
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(class UInputComponent* ic) {
	// Setup gameplay key binddings.
	check(ic);

	ic->BindAction("GrabLeft", IE_Pressed, this, &AVRPawn::GrabLeft);
	ic->BindAction("GrabLeft", IE_Released, this, &AVRPawn::StopGrabLeft);
	ic->BindAction("GrabRight", IE_Pressed, this, &AVRPawn::GrabRight);
	ic->BindAction("GrabRight", IE_Released, this, &AVRPawn::StopGrabRight);

	ic->BindAction("TeleportLeft", IE_Pressed, this, &AVRPawn::PreviewTeleportLeft);
	ic->BindAction("TeleportLeft", IE_Released, this, &AVRPawn::CompleteTeleportLeft);
	ic->BindAction("TeleportRight", IE_Pressed, this, &AVRPawn::PreviewTeleportRight);
	ic->BindAction("TeleportRight", IE_Released, this, &AVRPawn::CompleteTeleportRight);

	ic->BindAxis("MotionControllerThumbLeft_Y");
	ic->BindAxis("MotionControllerThumbLeft_X");
	ic->BindAxis("MotionControllerThumbRight_Y");
	ic->BindAxis("MotionControllerThumbRight_X");
	

	ic->BindAxis("MoveForward", this, &AVRPawn::MoveForward);
	ic->BindAxis("MoveRight", this, &AVRPawn::MoveRight);
	ic->BindAxis("LookUp", this, &AVRPawn::LookUp);
	ic->BindAxis("Turn", this, &AVRPawn::Turn);

}

AMotionControllerActor* AVRPawn::SpawnMotionController() {
	UWorld* const World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		return World->SpawnActor<AMotionControllerActor>(MotionControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	return nullptr;
}

void AVRPawn::GrabLeft() {
	LeftMotionController->GrabActor();
}

void AVRPawn::StopGrabLeft() {
	LeftMotionController->ReleaseActor();
}

void AVRPawn::GrabRight() {
	RightMotionController->GrabActor();
}

void AVRPawn::StopGrabRight() {
	RightMotionController->ReleaseActor();
}

void AVRPawn::PreviewTeleportLeft() {
	LeftMotionController->ActivateTeleporter();
	RightMotionController->DisableTeleporter();
}

void AVRPawn::CompleteTeleportLeft() {
	if (LeftMotionController->GetIsTeleporterActive()) {
		ExcuteTeleportation(LeftMotionController);
	}
}

void AVRPawn::PreviewTeleportRight() {
	RightMotionController->ActivateTeleporter();
	LeftMotionController->DisableTeleporter();
}

void AVRPawn::CompleteTeleportRight() {
	if (RightMotionController->GetIsTeleporterActive()) {
		ExcuteTeleportation(RightMotionController);
	}
}

void AVRPawn::UpdateTeleportRotation(AMotionControllerActor* MotionController) {
	if (MotionController->GetIsTeleporterActive()) {
		float Axis_Y;
		float Axis_X;
		if (MotionController->Hand == EControllerHand::Left) {
			Axis_Y = MotionController->GetInputAxisValue("MotionControllerThumbLeft_Y");
			Axis_X = MotionController->GetInputAxisValue("MotionControllerThumbLeft_X");
		}
		else {
			Axis_Y = MotionController->GetInputAxisValue("MotionControllerThumbRight_Y");
			Axis_X = MotionController->GetInputAxisValue("MotionControllerThumbRight_X");
		}
		FRotator TeleportRotation = GetRotationFromInput(Axis_Y, Axis_X, MotionController);
		MotionController->SetTeleportRotation(TeleportRotation);
	}
}

void AVRPawn::MoveForward(float Value)
{
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, Value);
}

void AVRPawn::MoveRight(float Value)
{
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	FVector Direction = GetActorRightVector();
	AddMovementInput(Direction, Value);
}

void AVRPawn::LookUp(float Value)
{
	FRotator NewRotation = Camera->GetRelativeTransform().Rotator();
	NewRotation.Pitch += Value;
	Camera->SetRelativeRotation(NewRotation);
}
void AVRPawn::Turn(float Value) 
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Value;
	SetActorRotation(NewRotation);
}

void AVRPawn::SearchTarget()
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	FVector Start = Camera->GetComponentLocation() + FVector(0, 0, -60);
	FVector End = Start + (Camera->GetComponentRotation().Vector()*1000.f);
	bool bHit = this->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Pawn, Params);
	if (bHit) {
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Yellow, "HitTarget: " + Hit.Actor.Get()->GetName());
		ATP_ThirdPersonCharacter* ThirdPerson = Cast<ATP_ThirdPersonCharacter>(Hit.Actor.Get());
		if (ThirdPerson) {
			ThirdPerson->Jump();
		}
	}
}

UPawnMovementComponent* AVRPawn::GetMovementComponent() const {
	return VRPawnMovementComponent;
}

void AVRPawn::ExcuteTeleportation(AMotionControllerActor* MotionController) {
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

FRotator AVRPawn::GetRotationFromInput(float UpAxis, float RightAxis, AMotionControllerActor* MotionController) {
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


