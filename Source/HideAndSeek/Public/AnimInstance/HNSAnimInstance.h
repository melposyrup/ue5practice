// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HNSAnimInstance.generated.h"

class AHideAndSeekCharacter;
class UPlayerMovement;


/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHNSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/** Begin() */
	virtual void NativeInitializeAnimation() override;

	/** Tick() */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	AHideAndSeekCharacter* HNSPlayer;

	UPROPERTY()
	UPlayerMovement* PlayerMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference , meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	void GetGroundSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bShouldMove;
	void GetShouldMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;
	void GetIsFalling();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing;
	void GetIsClimbing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reference, meta = (AllowPrivateAccess = "true"))
	FVector ClimbVelocity;
	void GetClimbVelocity();

};
