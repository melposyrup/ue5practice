// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HNSHUD.generated.h"


class UHNSOverlay;

UCLASS()
class HIDEANDSEEK_API AHNSHUD : public AHUD
{
	GENERATED_BODY()

public:
	void ShowWidget(APlayerController* PlayerController, UUserWidget* Wdiget);
	void HideWidget(APlayerController* PlayerController, UUserWidget* widget);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = HideAndSeek)
	TSubclassOf<UHNSOverlay> OverlayClass;

	UPROPERTY()
	UHNSOverlay* Overlay;

public:
	FORCEINLINE UHNSOverlay* GetHNSOverlay() const { return Overlay; }
};
