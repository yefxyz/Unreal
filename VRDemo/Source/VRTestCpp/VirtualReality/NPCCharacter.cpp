// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CharacterWidgetComponent.h"

#include "CharacterNameUserWidget.h"
#include "VRCharacter.h"
#include "NPCAIController.h"
#include "NPCCharacter.h"


// Sets default values
ANPCCharacter::ANPCCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WidgetRangeSphere"));
	WidgetRangeSphere->SetupAttachment(RootComponent);
	WidgetRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacter::OnWidgetSphereBeginOverlap);
	WidgetRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacter::OnWidgetShpereEndOverlap);
	WidgetRangeSphere->SetSphereRadius(500.0f);

	NameWidget = CreateDefaultSubobject<UCharacterWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(RootComponent);

	MainUIWidget = CreateDefaultSubobject<UCharacterWidgetComponent>(TEXT("MainUIWidget"));
	MainUIWidget->SetupAttachment(RootComponent);

	// Custom AIController.
	AIControllerClass = NPCAIControllerClass = ANPCAIController::StaticClass();
}

// Called when the game starts or when spawned
void ANPCCharacter::BeginPlay() {
	Super::BeginPlay();

	USkeletalMeshComponent* CharaMesh = GetMesh();
	if (CharaMesh) {
		float Offset = GetHeight() + CharaMesh->GetRelativeTransform().GetLocation().Z + NameWidgetDistance;
		NameWidget->SetHeight(Offset);

		// 名称设置。
		UCharacterNameUserWidget* NameUserWidget = Cast<UCharacterNameUserWidget>(NameWidget->GetUserWidgetObject());
		if (NameUserWidget) {
			NameUserWidget->SetNameText(Name);
		}
	}

	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
}

// Called every frame
void ANPCCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// 交互目标锁定高亮轮廓显示。
	GetMesh()->SetRenderCustomDepth(bInteractionTargeted);

	InteractionStateHandler();
}

// Called to bind functionality to input
void ANPCCharacter::SetupPlayerInputComponent(class UInputComponent* InputComp) {
	Super::SetupPlayerInputComponent(InputComp);

}

float ANPCCharacter::GetHeight() {
	float Height = 0.0f;
	USkeletalMeshComponent* CharaMesh = GetMesh();
	if (CharaMesh) {

		FVector Origin;
		FVector Extent;
		float Radius;
		FBoxSphereBounds MeshBounds;
		UKismetSystemLibrary::GetComponentBounds(CharaMesh, Origin, Extent, Radius);

		// for debug.
		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, FString::Printf(TEXT("Character Origin Vector. X: %f, Y: %f, Z: %f"), Origin.X, Origin.Y, Origin.Z));
		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, FString::Printf(TEXT("Character Box Extent Vector. X: %f, Y: %f, Z: %f"), Extent.X, Extent.Y, Extent.Z));
		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, FString::Printf(TEXT("Character Sphere Radius: %f"), Radius));

		Height = Extent.Z * 2;
		// GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, FString::Printf(TEXT("Character Height: %f"), Height));
	}
	return Height;
}

void ANPCCharacter::OnWidgetSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (OtherActor->IsA(AVRCharacter::StaticClass())) {
		// 玩家进入Widget显示范围。
		bShowWidget = true;
	}
}

void ANPCCharacter::OnWidgetShpereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA(AVRCharacter::StaticClass())) {
		// 玩家离开Widget显示范围。
		bShowWidget = false;
		InteractionState = EInteractionState::Dismissed;
	}
}

void ANPCCharacter::InteractionStateHandler() {
	MainUIWidget->SetShowWidget(false);
	switch (InteractionState) {
		case EInteractionState::None:
			break;
		case EInteractionState::Called:
		{
			// 移动到目标位置（一般为玩家面前）。
			FVector Dist2Dest = DestLocation - GetActorLocation();
			if (Dist2Dest.Size() > 10) {
				ANPCAIController* AINPCContr = Cast<ANPCAIController>(Controller);
				if (AINPCContr && GetVelocity().Size() == 0) {
					AINPCContr->MoveToLocation(DestLocation);
				}
			}
			else {
				InteractionState = EInteractionState::UIOpened;
			}
		}
		break;
		case EInteractionState::UIOpened:
		{
			MainUIWidget->SetShowWidget(true);
			break;
		}
		case EInteractionState::Dismissed:
		{
			MainUIWidget->SetShowWidget(false);
			// 返回初始位置。
			FVector DistOrigin = GetActorLocation() - OriginalLocation;
			if (DistOrigin.Size() > 10) {
				ANPCAIController* AINPCContr = Cast<ANPCAIController>(Controller);
				if (AINPCContr && GetVelocity().Size() == 0) {
					AINPCContr->MoveToLocation(OriginalLocation);
				}
			}
			break;
		}
		default:
			break;
	}
}
