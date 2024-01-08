// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HNSOverlay.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Animation/WidgetAnimation.h"

void UHNSOverlay::SetScoreText(int32 Score, int32 ScoreMax)
{
	if (ScoreText)
	{
		const FString StringScore = FString::Printf(TEXT("%d"), Score);
		const FString StringScoreMax = FString::Printf(TEXT("%d"), ScoreMax);
		const FText Text = FText::FromString(StringScore + "/" + StringScoreMax);
		ScoreText->SetText(Text);
	}
}

void UHNSOverlay::ShowOverlayBox(UHorizontalBox* box, bool bVisible)
{
	if (!box) return;

	if (bVisible)
	{
		box->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		box->SetVisibility(ESlateVisibility::Hidden);
	}

}


