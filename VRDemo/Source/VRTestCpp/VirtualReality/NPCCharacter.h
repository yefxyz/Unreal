// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "NPCCharacter.generated.h"


UENUM(BlueprintType, meta = (DisplayName = "Interaction State"))
enum class EInteractionState : uint8 {
	None,
	Called,
	UIOpened,
	Dismissed,
};

UCLASS()
/// <summary>
/// Class ANPCCharacter.
/// </summary>
/// <seealso cref="ACharacter" />
class VRTESTCPP_API ANPCCharacter : public ACharacter {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	TSubclassOf<class ANPCAIController> NPCAIControllerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	USphereComponent* WidgetRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	bool bShowWidget;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	///// <summary>
	///// 名称信息组件。
	///// </summary>
	//TSubclassOf<class UCharacterName> NameWidgetClass;

	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	//class UCharacterName* NameUMG;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UCharacterWidgetComponent* NameWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = true))
	float NameWidgetDistance = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Info", meta = (AllowPrivateAccess = true))
	FText Name = FText::FromString(TEXT("Marvin"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UCharacterWidgetComponent* MainUIWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = true))
	float InteractionDistance = 250.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// 交互目标锁定标识。
	/// </summary>
	bool bInteractionTargeted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// 交互状态。
	/// </summary>
	EInteractionState InteractionState;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector OriginalLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FRotator OriginalRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector DestLocation;

protected:
	UFUNCTION(Category = "Widget")
	void OnWidgetSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(Category = "Widget")
	void OnWidgetShpereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void InteractionStateHandler();

public:
	// Sets default values for this character's properties
	ANPCCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComp) override;

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	inline FText GetName() const { return Name; }

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	float GetHeight();

	inline bool IsShowWidget() const { return bShowWidget; }
	inline void SetShowWidget(bool val) { bShowWidget = val; }
	
	inline bool IsInteractionTargeted() const { return bInteractionTargeted; }
	inline void SetInteractionTargeted(bool val) { bInteractionTargeted = val; }
	
	inline EInteractionState GetInteractionState() const { return InteractionState; }
	inline void SetInteractionState(EInteractionState val) { InteractionState = val; }

	inline float GetInteractionDistance() const { return InteractionDistance; }
	inline void SetInteractionDistance(float val) { InteractionDistance = val; }
};
