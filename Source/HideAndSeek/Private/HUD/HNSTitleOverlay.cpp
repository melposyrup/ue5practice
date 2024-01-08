// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HNSTitleOverlay.h"
#include "GameFramework/Controller.h"

void UHNSTitleOverlay::UseMouseCursor()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* Controller = World->GetFirstPlayerController();
	if (!Controller) return;

	Controller->SetIgnoreMoveInput(true);
	Controller->SetIgnoreLookInput(true);
	Controller->SetShowMouseCursor(true);
}
