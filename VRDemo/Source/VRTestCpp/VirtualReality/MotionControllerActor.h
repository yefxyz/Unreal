// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "SteamVRChaperoneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MotionControllerActor.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "Grip State"))
enum class EGripState : uint8 {
	Open,
	CanGrab,
	Grab
};

UCLASS()
/// <summary>
/// Class AMotionControllerActor.
/// </summary>
/// <seealso cref="AActor" />
class VRTESTCPP_API AMotionControllerActor : public AActor {
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UArrowComponent* ArcDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USplineComponent* ArcSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USphereComponent* GrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* ArcEndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	/// <summary>
	/// 3D widget���������
	/// </summary>
	class UWidgetInteractionComponent* WidgetInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* TeleportCylinder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Ring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Arrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RoomScaleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USteamVRChaperoneComponent* SteamVRChaperone;

	UPROPERTY(BlueprintReadWrite, Category = "Motion Controller", meta = (AllowPrivateAccess = true))
	FRotator InitialControllerRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Motion Controller", meta = (AllowPrivateAccess = true))
	bool IsRoomScale;

	UPROPERTY(BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = true))
	AActor* AttachedActor;

	UPROPERTY(BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = true))
	EGripState GripState;

	UPROPERTY(BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = true))
	bool bWantsToGrip;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	bool IsTeleporterActive;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	TArray<USplineMeshComponent*> SplineMeshes;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	bool bLastFrameValidDestination;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	FVector TeleportDestination;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	FRotator TeleportRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	float TeleportLaunchVelocity = 900.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	bool IsValidTeleportDestination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleportation", meta = (AllowPrivateAccess = true))
	float DisableDelayValue = 0.2f;

	FTimerHandle DisableDelayHandle;

protected:
	UFUNCTION(Category = "Grabbing")
	void OnGraSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(Category = "Grabbing")
	void OnHandMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult);

public:
	// Sets default values for this actor's properties
	AMotionControllerActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller", meta = (ExposeOnSpawn = true))
	EControllerHand Hand;

	void init(EControllerHand h);

	// ---- Room Scale Start ---- //
	//UFUNCTION(BlueprintCallable, Category = "Room Scale")
	//void SetupRoomScaleOutline();

	UFUNCTION(BlueprintCallable, Category = "Room Scale")
	/// <summary>
	/// ���·�������������
	/// �Գ�HMDλ�ơ�
	/// </summary>
	void UpdateRoomScaleOutline();
	// ---- Room Scale End ---- //

	// ---- Grabbing Start ---- //
	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// ��ȡ��ӽ��Ŀ�ʰȡ�ʵ����PickupInterface����
	/// </summary>
	/// <returns>AActor *.</returns>
	AActor* GetActorNearHand();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// ץס���塣
	/// </summary>
	void GrabActor();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// �ɿ����塣
	/// </summary>
	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// �𶯿�������
	/// </summary>
	/// <param name="Intensity">The intensity.</param>
	void RumbleController(const float Intensity);
	
	UFUNCTION()
	void RumbleLatentAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grabbing")
	/// <summary>
	/// ����˫�ֶ���״̬��BP��ʵ�֡�
	/// </summary>
	void UpdateHandAnimation();
	// ---- Grabbing End ---- //

	// ---- Teleportation Start ---- //
	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// ���ô��͡�
	/// </summary>
	void ActivateTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// ͣ�ô��͡�
	/// </summary>
	void DisableTeleporter();

	void DelayDisableTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// ��ȡ����Ŀ��㡣
	/// </summary>
	/// <param name="OutLocation">The location.</param>
	/// <param name="OutRotation">The rotation.</param>
	void GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// Ͷ�������ߣ�Ѱ�Ҵ���Ŀ��㡣
	/// </summary>
	/// <param name="OutTracePoints">The trace points.</param>
	/// <param name="OutNavMeshLocation">The nav mesh location.</param>
	/// <param name="OutTraceLocation">The trace location.</param>
	/// <returns>bool.</returns>
	bool TraceTeleportDestination(TArray<FVector>& OutTracePoints, FVector& OutNavMeshLocation, FVector& OutTraceLocation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// �������������ߡ�
	/// </summary>
	/// <param name="SplinePoints">The spline points.</param>
	/// <returns>bool.</returns>
	void UpdateArcSpline(const bool FoundValidLocation, UPARAM(ref) TArray<FVector>& SplinePoints);

	UFUNCTION(BlueprintImplementableEvent, Category = "Teleportation")
	/// <summary>
	/// ����ָ����״����ʵ����������ߡ�BP��ʵ�֡�
	/// </summary>
	void GenerateArcSplineMesh(const TArray<FVector>& SplinePoints);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// �����������յ㡣
	/// </summary>
	/// <param name="NewLocation">The new location.</param>
	/// <returns>bool.</returns>
	void UpdateArcEndpoint(const bool FoundValidLocation, UPARAM(ref) FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// ��������ߡ�
	/// </summary>
	void ClearArc();
	// ---- Teleportation End ---- //

	// ---- Properties ---- //
	inline bool GetIsTeleporterActive() const { return IsTeleporterActive; }
	inline void SetIsTeleporterActive(bool val) { IsTeleporterActive = val; }

	inline FRotator GetInitialControllerRotation() const { return InitialControllerRotation; }
	inline void SetInitialControllerRotation(FRotator val) { InitialControllerRotation = val; }
	
	inline bool GetIsValidTeleportDestination() const { return IsValidTeleportDestination; }
	inline void SetIsValidTeleportDestination(bool val) { IsValidTeleportDestination = val; }
	
	inline FRotator GetTeleportRotation() const { return TeleportRotation; }
	inline void SetTeleportRotation(FRotator val) { TeleportRotation = val; }
};
