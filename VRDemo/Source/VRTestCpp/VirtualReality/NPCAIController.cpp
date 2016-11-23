// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"

#include "NPCCharacter.h"
#include "NPCAIController.h"


void ANPCAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	ANPCCharacter* NPC = Cast<ANPCCharacter>(GetCharacter());
	if (NPC) {
		if (NPC->GetInteractionState() == EInteractionState::Called) {
			// ����Ŀ�ĵغ��UI��
			NPC->SetInteractionState(EInteractionState::UIOpened);
		}
		else if (NPC->GetInteractionState() == EInteractionState::Dismissed) {
			// �ָ���ʼ����
			NPC->SetActorRotation(NPC->OriginalRotation);
		}
	}
}
