// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HNSOverlay.generated.h"

class UTextBlock;
class UHorizontalBox;
class UWidgetAnimation;

UCLASS()
class HIDEANDSEEK_API UHNSOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetScoreText(int32 Score, int32 ScoreMax);
	void ShowOverlayBox(UHorizontalBox* box, bool bVisible);
private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Sprint;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Climb;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* JumpOff;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Interact;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* GameClear;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* GameClearFadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* GetIntoGameScene;

public:
	FORCEINLINE UHorizontalBox* GetSprintTutorialBox() const { return Sprint; }
	FORCEINLINE UHorizontalBox* GetClimbTutorialBox() const { return Climb; }
	FORCEINLINE UHorizontalBox* GetJumpOffTutorialBox() const { return JumpOff; }
	FORCEINLINE UHorizontalBox* GetInteractTutorialBox() const { return Interact; }
	FORCEINLINE UHorizontalBox* GetGameClearBox() const { return GameClear; }
	FORCEINLINE UWidgetAnimation* GetGameClearFadeIn() const { return GameClearFadeIn; }
	FORCEINLINE UWidgetAnimation* GetGetIntoGameScene() const { return GetIntoGameScene; }
};
