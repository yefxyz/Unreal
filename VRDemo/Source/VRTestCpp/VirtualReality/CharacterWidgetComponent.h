// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "CharacterWidgetComponent.generated.h"

UCLASS(BlueprintType)
/// <summary>
/// Class UCharacterWidgetComponent.
/// </summary>
/// <seealso cref="UWidgetComponent" />
class VRTESTCPP_API UCharacterWidgetComponent : public UWidgetComponent {
	GENERATED_BODY()

private:
	// ��UI��TODO

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// Widget��Y����NPC��ɫ����Ծ��롣
	/// </summary>
	float Radius = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// Widget��Z����NPC��ɫ����Ծ��롣
	/// </summary>
	float Height = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	/// <summary>
	/// Widget���ű��ʡ�
	/// </summary>
	float Scale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = true))
	bool bShowWidget = true;

public:
	UCharacterWidgetComponent();

	virtual void PostLoad() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	inline float GetRadius() const { return Radius; }
	inline void SetRadius(float val) { Radius = val; }

	inline float GetHeight() const { return Height; }
	inline void SetHeight(float val) { Height = val; }
	
	inline bool IsShowWidget() const { return bShowWidget; }
	inline void SetShowWidget(bool val) { bShowWidget = val; }
};
