// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteraction.generated.h"

class AHNSPawnTarget;
class AHideAndSeekCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HIDEANDSEEK_API UPlayerInteraction : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInteraction();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact(AHNSPawnTarget* OverlappingTarget);
	FHitResult Raycast();

protected:
	virtual void BeginPlay() override;

	void RaycastInteraction();

private:
	AHideAndSeekCharacter* Owner;

};
