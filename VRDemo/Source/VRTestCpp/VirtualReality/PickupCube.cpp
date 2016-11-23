// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestCpp.h"
#include "PickupCube.h"


APickupCube::APickupCube() {

	UStaticMeshComponent* Mesh = GetStaticMeshComponent();
	RootComponent = Mesh;

	// Transform.
	const FVector Scale = FVector(0.2f);
	Mesh->SetWorldScale3D(Scale);
	Mesh->SetMobility(EComponentMobility::Movable);

	// Physics.
	Mesh->SetSimulatePhysics(true);

	// Collision.
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->bGenerateOverlapEvents = true;
	Mesh->SetCollisionProfileName(FName("PhysicsActor"));
}

void APickupCube::Pickup_Implementation(USceneComponent* Picker) {
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(RootComponent);
	Mesh->SetSimulatePhysics(false);

	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
	RootComponent->AttachToComponent(Picker, rules);
}

void APickupCube::Drop_Implementation() {
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(RootComponent);
	Mesh->SetSimulatePhysics(true);

	FDetachmentTransformRules rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	DetachFromActor(rules);
}
