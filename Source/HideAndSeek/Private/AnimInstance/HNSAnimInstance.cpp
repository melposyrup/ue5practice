// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/HNSAnimInstance.h"
#include "HideAndSeek/HideAndSeekCharacter.h"
#include "Components/PlayerMovement.h"
#include "Kismet/KismetMathLibrary.h"



void UHNSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HNSPlayer = Cast<AHideAndSeekCharacter>(TryGetPawnOwner());
	if (HNSPlayer)
	{
		PlayerMovement = Cast<UPlayerMovement>(HNSPlayer->GetPlayerMovemenntComponent());
	}
}

void UHNSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!HNSPlayer || !PlayerMovement) return;

	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsClimbing();
	GetClimbVelocity();
}

void UHNSAnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(HNSPlayer->GetVelocity());
}

void UHNSAnimInstance::GetAirSpeed()
{
	AirSpeed = HNSPlayer->GetVelocity().Z;
}

void UHNSAnimInstance::GetShouldMove()
{
	bShouldMove =
		PlayerMovement->GetCurrentAcceleration().Size() > 0 &&
		GroundSpeed > 5.f &&
		!bIsFalling;
}

void UHNSAnimInstance::GetIsFalling()
{
	bIsFalling = PlayerMovement->IsFalling();
}

void UHNSAnimInstance::GetIsClimbing()
{
	bIsClimbing = PlayerMovement->IsClimbing();
}

void UHNSAnimInstance::GetClimbVelocity()
{
	ClimbVelocity = PlayerMovement->GetUnrotatedClimbVelocity();
}
