// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovement.generated.h"

class UAnimMontage;
class UAnimInstance;
class AHideAndSeekCharacter;

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
	};
}


UCLASS()
class HIDEANDSEEK_API UPlayerMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()


public:
#pragma region OverrridenFunctions

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxAcceleration() const override;

	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const;

#pragma endregion



#pragma region ClimbCore

public:

	void ToggleClimbing(bool bEnableClimbing);

	bool IsClimbing() const;

	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return CurrentClimbableSurfaceNormal; }

	FVector GetUnrotatedClimbVelocity() const;

	bool IsClimbButtonVaild();


private:

	bool TraceClimbableSurfaces();

	FHitResult TraceFromEyeHeight(float TraceDistance, float TraceStartOffset = 0.f);

	bool CanStartClimbing();

	bool CanClimbDownLedge();

	void StartClimbing();

	void StopClimbing();

	void PhysClimb(float deltaTime, int32 Iterations);

	void ProcessClimbableSurafaceInfo();

	void AverageClimbableSurfaceLocationAndNormal();

	void NearesrClimbableSurfaceLocationAndNormal();

	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();

	FQuat GetClimbRotation(float deltaTime);

	void SnapMovementToClimbableSurfaces(float deltaTime);

	bool CheckHasReachedLedge();

	bool CheckHasSomethingAboveHead();

	void PlayClimbMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted);

#pragma endregion



#pragma region ClimbCoreVariables

	TArray<FHitResult> ClimbableSurfaceTracedResults;

	FVector CurrentClimbableSurfaceLocation;

	FVector CurrentClimbableSurfaceNormal;

	UPROPERTY()
	UAnimInstance* OwningPlayerAnimInstance;

#pragma endregion



#pragma region ClimbTraces

	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape = false, bool bDrawPersistentShapes = false);

	FHitResult DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape = false, bool bDrawPersistentShapes = false);

#pragma endregion



#pragma region ClimbBPVariables

	float ClimbDownTraceDistanceDownload = 200.f;	/* revise to 100.f when applied to the Cat */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbDownLedgeTraceOffset = 30.f;	/* revise to 10.f when applied to the Cat */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbDownWalkableSurfaceTraceOffset = 10.f; /* revise to 30.f when applied to the Cat */

	float WalkableSurfaceDownOffset = 60.f;	/* revise to 40.f when applied to the Cat */

	float TraceOffsetFromEyeHeight = 30.f;	/* revise to .f when applied to the Cat */

	float FloorCheckDistance = 100.f;	/* revise to 40.f when applied to the Cat */

	float DegreeDiffToExitClimbing = 30.f;	/* can keep 60.f when applied to the Cat */

	float ClimbCapsuleTraceForwardOffset = 20.f;	/* revise to 10.f when applied to the Cat */

	float TraceDistanceFromEyeHeight = 60.f;	/* revise to 30.f when applied to the Cat */

	float ClimbCapsuleTraceRadius = 40.0f;	/* revise to 15.f when applied to the Cat */

	float ClimbCapsuleTraceHalfHeight = 60.0f;	/* revise to 24.f when applied to the Cat */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > ClimbableSurfaceTraceTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxBreakClimbDeceleration = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxClimbSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxClimbAcceleration = 800.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbDownLedgeMontage;

	AHideAndSeekCharacter* Player;

#pragma endregion



};
