// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerInteraction.h"
#include "Interfaces/HNSInteractInterface.h"
#include "HideAndSeek/DebugHelper.h"
#include "HideAndSeek/HideAndSeekCharacter.h"
#include "HNSPawnTarget.h"


UPlayerInteraction::UPlayerInteraction()
{
	PrimaryComponentTick.bCanEverTick = false;

	Owner = Cast<AHideAndSeekCharacter>(GetOwner());
}

void UPlayerInteraction::BeginPlay()
{
	Super::BeginPlay();
}


void UPlayerInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerInteraction::Interact(AHNSPawnTarget* OverlappingTarget)
{
	if (!Owner) return;

	//if has OverlappingTarget, call OverlappingTarget implementation
	if (OverlappingTarget && OverlappingTarget->Implements<UHNSInteractInterface>())
	{
		IHNSInteractInterface::Execute_Interact(OverlappingTarget, Owner);
	}
	else
	{
		// else use raycast to call implementation
		RaycastInteraction();
	}

}
FHitResult UPlayerInteraction::Raycast()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * 800.f);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(HitResult, EyeLocation, End, ObjectQueryParams);

	DRAW_LINE_SingleFrame(EyeLocation, End);

	return HitResult;
}

void UPlayerInteraction::RaycastInteraction()
{
	AActor* HitActor = Raycast().GetActor();

	if (HitActor && HitActor->Implements<UHNSInteractInterface>())
	{
		IHNSInteractInterface::Execute_Interact(HitActor, Owner);
	}

}

