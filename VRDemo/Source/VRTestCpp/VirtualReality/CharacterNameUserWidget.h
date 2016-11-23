// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CharacterNameUserWidget.generated.h"

UCLASS()
/// <summary>
/// Class UCharacterNameUserWidget.
/// </summary>
/// <seealso cref="UUserWidget" />
class VRTESTCPP_API UCharacterNameUserWidget : public UUserWidget {
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowProtectedAccess = true))
	FText NameText;

public:
	inline FText GetNameText() const { return NameText; }
	inline void SetNameText(FText val) { NameText = val; }
};
