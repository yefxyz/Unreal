// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WidgetInteractionComponent.h"
#include "AIController.h"

#include "VRCharacter.h"
#include "NPCCharacter.h"
#include "PickupActorInterface.h"
#include "MotionControllerActor.h"


// Sets default values
AMotionControllerActor::AMotionControllerActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(RootComponent);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMesh->SetupAttachment(MotionController);
	HandMesh->OnComponentHit.AddDynamic(this, &AMotionControllerActor::OnHandMeshHit);

	ArcDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ArcDirection"));
	ArcDirection->SetupAttachment(HandMesh);

	ArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	ArcSpline->SetupAttachment(HandMesh);

	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
	GrabSphere->SetupAttachment(HandMesh);
	GrabSphere->OnComponentBeginOverlap.AddDynamic(this, &AMotionControllerActor::OnGraSphereBeginOverlap);

	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndPoint"));
	ArcEndPoint->SetupAttachment(MotionController);

	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteraction->SetupAttachment(MotionController);

	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCylinder"));
	TeleportCylinder->SetupAttachment(RootComponent);

	Ring = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
	Ring->SetupAttachment(TeleportCylinder);

	Arrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(TeleportCylinder);

	RoomScaleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomScaleMesh"));
	RoomScaleMesh->SetupAttachment(Arrow);

	SteamVRChaperone = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("SteamVRChaperone"));
}

// Called when the game starts or when spawned
void AMotionControllerActor::BeginPlay() {
	Super::BeginPlay();

	// Init Room Scale. ��ͼ����ɡ�

	TeleportCylinder->SetVisibility(false, true);
}

// Called every frame
void AMotionControllerActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// ����˫��״̬�����Ŷ�����
	if (AttachedActor || bWantsToGrip) {
		GripState = EGripState::Grab;
	}
	else if (GetActorNearHand()) {
		GripState = EGripState::CanGrab;
	}
	else {
		GripState = EGripState::Open;
	}
	UpdateHandAnimation();

	// ���������ߴ���
	ClearArc();
	if (IsTeleporterActive) {
		TArray<FVector> TracePoints;
		FVector NavMeshPosition;
		FVector TraceLocation;
		IsValidTeleportDestination = TraceTeleportDestination(TracePoints, NavMeshPosition, TraceLocation);

		// ����TeleportCylinder��
		TeleportCylinder->SetVisibility(IsValidTeleportDestination, true);
		TArray<TEnumAsByte<EObjectTypeQuery>> QueryTypes = { EObjectTypeQuery::ObjectTypeQuery1 };
		FHitResult HitResult;
		const TArray<AActor*> NONEActors;
		FVector EndPoint = NavMeshPosition + FVector(0, 0, -200);
		// NavMesh���ڵ��棬��̽�ҵ�ʵ�ʵ����Ӧ�㡣
		UKismetSystemLibrary::LineTraceSingleForObjects(this, NavMeshPosition, EndPoint, QueryTypes, false, NONEActors, EDrawDebugTrace::None, HitResult, true);
		if (HitResult.IsValidBlockingHit()) {
			TeleportCylinder->SetWorldLocation(HitResult.ImpactPoint);
		}
		else {
			TeleportCylinder->SetWorldLocation(NavMeshPosition);
		}

		// �����͵��Ƿ���Ч�����仯���𶯿�������
		if (IsValidTeleportDestination && !bLastFrameValidDestination || !IsValidTeleportDestination && bLastFrameValidDestination) {
			RumbleController(0.3f);
		}
		bLastFrameValidDestination = IsValidTeleportDestination;

		UpdateArcSpline(IsValidTeleportDestination, TracePoints);
		UpdateArcEndpoint(IsValidTeleportDestination, TraceLocation);
	}

	// ���·�������������
	UpdateRoomScaleOutline();

	// ֻ��ץȡ��Ʒ״̬����˫����ײ��
	if (GripState == EGripState::Open || GripState == EGripState::CanGrab) {
		HandMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
	else {
		HandMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	}

	// ���ִ򿪹켣���ߡ�
	if (Hand == EControllerHand::Right) {
		AVRCharacter* VRChara = Cast<AVRCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		const TArray<AActor*> IgnoreActors = { this, VRChara, VRChara->GetLeftMotionController() };
		FVector Start = HandMesh->GetComponentLocation();
		FVector End = Start + (HandMesh->GetComponentRotation().Vector() * WidgetInteraction->InteractionDistance);
		VRChara->OpenTraceLine(Start, End, IgnoreActors);
	}
}

void AMotionControllerActor::init(EControllerHand h) {
	// Set Motion Controller Hand.
	MotionController->Hand = Hand = h;
	if (Hand == EControllerHand::Left) {
		HandMesh->SetWorldScale3D(FVector(1, 1, -1));
	}
}
/*
void AMotionControllerActor::SetupRoomScaleOutline() {
	//TArray<FVector> RoomBounds;

	FVector RectCenter;
	FRotator RectRotation;
	float SideLengthX;
	float SideLengthY;

	float ChaperoneMeshHeight = 70.0f;

	UKismetMathLibrary::MinimumAreaRectangle(this, SteamVRChaperone->GetBounds(), FVector::UpVector, RectCenter, RectRotation, SideLengthX, SideLengthY);

	if (!UKismetMathLibrary::NearlyEqual_FloatFloat(SideLengthX, 100, 0.01f) || !UKismetMathLibrary::NearlyEqual_FloatFloat(SideLengthY, 100, 0.01f)) {
		IsRoomScale = true;

		FVector Scale = FVector(SideLengthX, SideLengthY, ChaperoneMeshHeight);
		RectRotation.RotateVector(Scale);
		RoomScaleMesh->SetWorldScale3D(Scale);
	}
}
*/
void AMotionControllerActor::UpdateRoomScaleOutline() {
	if (RoomScaleMesh->IsVisible()) {
		// HMD���Cameraλ����Ϣ��
		FRotator DeviceRotation;
		FVector DevicePosition;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
		// ��HMD��λ��Ͷ�䵽XYƽ�棬ȡ��Z��ҡ�ڣ�Yaw���ķ������λ�ã�ʹ�ô���Ŀ�귶Χ����HMD��ҡ�ڶ��仯��
		FVector UnrotateVector = FRotator(0, DeviceRotation.Yaw, 0).UnrotateVector(FVector(DevicePosition.X, DevicePosition.Y, 0));
		RoomScaleMesh->SetRelativeLocation(UnrotateVector);
	}
}

AActor* AMotionControllerActor::GetActorNearHand() {
	FVector GrabSpherePosition = GrabSphere->GetComponentLocation();

	float NearestOverlap = 10000.0f;
	AActor* NearestOverlappingActor = nullptr;

	TArray<AActor*> TouchedActors;
	GrabSphere->GetOverlappingActors(TouchedActors);
	for (int32 i = 0; i < TouchedActors.Num(); i++) {
		AActor* TouchedActor = TouchedActors[i];
		float Length = UKismetMathLibrary::VSize(UKismetMathLibrary::Subtract_VectorVector(TouchedActor->GetActorLocation(), GrabSpherePosition));
		if (UKismetSystemLibrary::DoesImplementInterface(TouchedActor, UPickupActorInterface::StaticClass())
			&& Length < NearestOverlap) {
			NearestOverlappingActor = TouchedActor;
			NearestOverlap = Length;
		}
	}
	return NearestOverlappingActor;
}

void AMotionControllerActor::GrabActor() {
	bWantsToGrip = true;
	AActor* Nearest = GetActorNearHand();
	if (Nearest) {
		AttachedActor = Nearest;
		IPickupActorInterface* Pickupable = Cast<IPickupActorInterface>(AttachedActor);
		Pickupable->Pickup(MotionController);
		RumbleController(0.7f);
	}
}

void AMotionControllerActor::ReleaseActor() {
	bWantsToGrip = false;
	if (AttachedActor && AttachedActor->GetRootComponent()->GetAttachParent() == MotionController) {
		IPickupActorInterface* Pickupable = Cast<IPickupActorInterface>(AttachedActor);
		Pickupable->Drop();
		RumbleController(0.2);
	}
	AttachedActor = nullptr;
}

void AMotionControllerActor::RumbleController(const float Intensity) {
	bool IsLeftHand = Hand == EControllerHand::Left;
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName(TEXT("RumbleLatentAction"));
	LatentInfo.UUID = FMath::FRand() * 100000;
	LatentInfo.Linkage = 0;
	Controller->PlayDynamicForceFeedback(Intensity, 0.05f, IsLeftHand, IsLeftHand, !IsLeftHand, !IsLeftHand, EDynamicForceFeedbackAction::Start, LatentInfo);
}

void AMotionControllerActor::RumbleLatentAction() {
	// Do nothing.
}

void AMotionControllerActor::ActivateTeleporter() {
	IsTeleporterActive = true;
	TeleportCylinder->SetVisibility(true, true);
	RoomScaleMesh->SetVisibility(IsRoomScale, false);

	// ������������ʱ������Rotation����������������������Ƚϣ���֧���ÿ�������ת���������ͳ���
	InitialControllerRotation = MotionController->GetComponentRotation();
}

void AMotionControllerActor::DisableTeleporter() {
	if (IsTeleporterActive) {
		IsTeleporterActive = false;
		TeleportCylinder->SetVisibility(false, true);
		ArcEndPoint->SetVisibility(false, false);
	}
	if (GetWorldTimerManager().IsTimerActive(DisableDelayHandle)) {
		GetWorldTimerManager().ClearTimer(DisableDelayHandle);
	}
}

void AMotionControllerActor::DelayDisableTeleporter() {
	GetWorldTimerManager().SetTimer(DisableDelayHandle, this, &AMotionControllerActor::DisableTeleporter, DisableDelayValue, false);
}

void AMotionControllerActor::GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation) {
	// HMD���Cameraλ����Ϣ��
	FVector HMDPosition;
	FRotator HMDRotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDPosition);
	FVector HMDXYPosition = TeleportRotation.RotateVector(FVector(HMDPosition.X, HMDPosition.Y, 0));
	// ��ȥHMD���λ�ƣ��õ�ȷ�е�Pawn����λ�á�
	OutLocation = TeleportCylinder->GetComponentLocation() - HMDXYPosition;
	OutRotation = TeleportRotation;
}

bool AMotionControllerActor::TraceTeleportDestination(TArray<FVector>& OutTracePoints, FVector& OutNavMeshLocation, FVector& OutTraceLocation) {
	FVector ArcStartPos = ArcDirection->GetComponentLocation();
	FVector ArcLaunchVelocity = ArcDirection->GetForwardVector() * TeleportLaunchVelocity;
	// Query World Static only.
	TArray<TEnumAsByte<EObjectTypeQuery>> QueryTypes = { EObjectTypeQuery::ObjectTypeQuery1 };

	FHitResult HitResult;
	FVector LastTraceDestination;
	const TArray<AActor*> NONEActors;
	// ģ��Ͷ����������Ѱ�Ҵ���Ŀ��㡣
	bool Success = UGameplayStatics::PredictProjectilePath(this, HitResult, OutTracePoints, LastTraceDestination, ArcStartPos, ArcLaunchVelocity, true, 0.0f, QueryTypes, false, NONEActors, EDrawDebugTrace::Type::None, 0.0f);
	OutTraceLocation = HitResult.Location;

	FVector QueryExtent = FVector(500.0f, 500.0f, 500.0f);
	// ���е�Ͷ�䵽NavMesh��
	OutNavMeshLocation = UNavigationSystem::ProjectPointToNavigation(this, OutTraceLocation, NULL, 0, QueryExtent);

	return Success && UKismetMathLibrary::NotEqual_VectorVector(OutTraceLocation, OutNavMeshLocation, 0.0001f);
}

void AMotionControllerActor::UpdateArcSpline(const bool FoundValidLocation, TArray<FVector>& SplinePoints) {
	if (FoundValidLocation) {
		// Ѱ�Ҵ��͵�ɹ����������������ߡ�
		GenerateArcSplineMesh(SplinePoints);
	}
	else {
		// Ѱ�Ҵ���Ŀ���ʧ�ܣ�������һ���߶Ρ�
		SplinePoints.Empty();
		FVector StartPos = ArcDirection->GetComponentLocation();
		FVector EndPos = StartPos + ArcDirection->GetForwardVector() * 20;
		SplinePoints.Add(StartPos);
		SplinePoints.Add(EndPos);
		GenerateArcSplineMesh(SplinePoints);
	}
}

void AMotionControllerActor::UpdateArcEndpoint(const bool FoundValidLocation, FVector& NewLocation) {
	// �������յ㡣
	ArcEndPoint->SetVisibility(IsTeleporterActive && FoundValidLocation, false);
	ArcEndPoint->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator HMDRotation;
	FVector HMDPosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDPosition);
	// �����ת��Ϣ����ȷָʾ���ͺ������Եķ���
	FRotator ArrowRotation = UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0, HMDRotation.Yaw, 0));
	Arrow->SetWorldRotation(ArrowRotation);

	RoomScaleMesh->SetWorldRotation(TeleportRotation);
}

void AMotionControllerActor::ClearArc() {
	for (int32 i = 0; i < SplineMeshes.Num(); i++) {
		SplineMeshes[i]->DestroyComponent();
	}
	SplineMeshes.Empty();
	ArcSpline->ClearSplinePoints(true);
}

void AMotionControllerActor::OnGraSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(OtherComp);
	if (Mesh && Mesh->IsSimulatingPhysics()) {
		RumbleController(0.8f);
	}
}

void AMotionControllerActor::OnHandMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult) {
	float Intensity = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(NormalImpulse), 0.0f, 1500.0f, 0.0f, 0.8f);
	if (Intensity > 0) {
		RumbleController(Intensity);
	}
}
