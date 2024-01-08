// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerAttributes.h"
#include "HideAndSeek/DebugHelper.h"

// Sets default values for this component's properties
UPlayerAttributes::UPlayerAttributes()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UPlayerAttributes::BeginPlay()
{
	Super::BeginPlay();
}


void UPlayerAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerAttributes::AddScore()
{
	Score++;
	FString ScoreString = FString::FromInt(Score);
	Debug::Print("Score :" + ScoreString, FColor::Cyan, 1);
}

bool UPlayerAttributes::IsLevelClear()
{
	return Score >= MaxScore;
}