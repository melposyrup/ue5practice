// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HNSTitleHUD.h"
#include "HUD/HNSTitleOverlay.h"
#include "GameFramework/Controller.h"
#include "HideAndSeek/HideAndSeekCharacter.h"

void AHNSTitleHUD::BeginPlay()
{
	Super::BeginPlay();

	/** instatiate widgetBP */
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* Controller = World->GetFirstPlayerController();
	if (!Controller) return;

	if (OverlayClass)
	{
		Overlay = CreateWidget<UHNSTitleOverlay>(Controller, OverlayClass);
		Overlay->AddToViewport();
		Overlay->UseMouseCursor();
	}
}
