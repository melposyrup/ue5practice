// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HNSTitleHUD.generated.h"

class UHNSTitleOverlay;

UCLASS()
class HIDEANDSEEK_API AHNSTitleHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = HideAndSeek)
	TSubclassOf<UHNSTitleOverlay> OverlayClass;

	UPROPERTY()
	UHNSTitleOverlay* Overlay;
};
