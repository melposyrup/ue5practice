// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HNSHUD.h"
#include "HUD/HNSOverlay.h"
#include "HideAndSeek/DebugHelper.h"

void AHNSHUD::BeginPlay()
{
	Super::BeginPlay();

	/** instatiate widgetBP */
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* Controller = World->GetFirstPlayerController();
	if (!Controller) return;

	if (OverlayClass)
	{
		Overlay = CreateWidget<UHNSOverlay>(Controller, OverlayClass);
		Overlay->AddToViewport();
	}
}

void AHNSHUD::ShowWidget(APlayerController* PlayerController, UUserWidget* Wdiget)
{
	if (Wdiget && PlayerController)
	{
		Wdiget->SetVisibility(ESlateVisibility::Visible);

		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetShowMouseCursor(true);

		Debug::Print("ShowWidget", FColor::MakeRandomColor(), 1);
	}
}

void AHNSHUD::HideWidget(APlayerController* PlayerController, UUserWidget* Wdiget)
{
	if (Wdiget && PlayerController)
	{
		Wdiget->SetVisibility(ESlateVisibility::Hidden);

		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->ResetIgnoreLookInput();
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());

		Debug::Print("HideWidget", FColor::MakeRandomColor(), 2);
	}
}