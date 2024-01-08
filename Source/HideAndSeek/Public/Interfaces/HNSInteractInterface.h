// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HNSInteractInterface.generated.h"

class AHideAndSeekCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHNSInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HIDEANDSEEK_API IHNSInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Interact(AHideAndSeekCharacter* Player);
};
