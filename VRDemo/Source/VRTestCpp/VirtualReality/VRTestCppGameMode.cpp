// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "VRTestCppGameMode.h"


AVRTestCppGameMode::AVRTestCppGameMode() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VirtualRealityCPP/Blueprints/BP_VRCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
