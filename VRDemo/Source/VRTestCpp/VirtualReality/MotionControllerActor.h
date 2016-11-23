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
	/// 3D widget交互组件。
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
	/// 更新房间区域轮廓。
	/// 对冲HMD位移。
	/// </summary>
	void UpdateRoomScaleOutline();
	// ---- Room Scale End ---- //

	// ---- Grabbing Start ---- //
	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// 获取最接近的可拾取物（实现了PickupInterface）。
	/// </summary>
	/// <returns>AActor *.</returns>
	AActor* GetActorNearHand();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// 抓住物体。
	/// </summary>
	void GrabActor();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// 松开物体。
	/// </summary>
	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	/// <summary>
	/// 震动控制器。
	/// </summary>
	/// <param name="Intensity">The intensity.</param>
	void RumbleController(const float Intensity);
	
	UFUNCTION()
	void RumbleLatentAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grabbing")
	/// <summary>
	/// 更新双手动画状态。BP端实现。
	/// </summary>
	void UpdateHandAnimation();
	// ---- Grabbing End ---- //

	// ---- Teleportation Start ---- //
	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 启用传送。
	/// </summary>
	void ActivateTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 停用传送。
	/// </summary>
	void DisableTeleporter();

	void DelayDisableTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 获取传送目标点。
	/// </summary>
	/// <param name="OutLocation">The location.</param>
	/// <param name="OutRotation">The rotation.</param>
	void GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 投射抛物线，寻找传送目标点。
	/// </summary>
	/// <param name="OutTracePoints">The trace points.</param>
	/// <param name="OutNavMeshLocation">The nav mesh location.</param>
	/// <param name="OutTraceLocation">The trace location.</param>
	/// <returns>bool.</returns>
	bool TraceTeleportDestination(TArray<FVector>& OutTracePoints, FVector& OutNavMeshLocation, FVector& OutTraceLocation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 更新抛物线曲线。
	/// </summary>
	/// <param name="SplinePoints">The spline points.</param>
	/// <returns>bool.</returns>
	void UpdateArcSpline(const bool FoundValidLocation, UPARAM(ref) TArray<FVector>& SplinePoints);

	UFUNCTION(BlueprintImplementableEvent, Category = "Teleportation")
	/// <summary>
	/// 生成指定形状与材质的抛物线曲线。BP端实现。
	/// </summary>
	void GenerateArcSplineMesh(const TArray<FVector>& SplinePoints);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 更新抛物线终点。
	/// </summary>
	/// <param name="NewLocation">The new location.</param>
	/// <returns>bool.</returns>
	void UpdateArcEndpoint(const bool FoundValidLocation, UPARAM(ref) FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Teleportation")
	/// <summary>
	/// 清除抛物线。
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
