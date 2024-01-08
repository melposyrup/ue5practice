// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerMovement.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HideAndSeek/HideAndSeekCharacter.h"
#include "HideAndSeek/DebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HUD/HNSOverlay.h"

#pragma region OverrridenFunctions


void UPlayerMovement::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AHideAndSeekCharacter>(GetOwner());

	OwningPlayerAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	if (OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UPlayerMovement::OnClimbMontageEnded);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UPlayerMovement::OnClimbMontageEnded);
	}
}

void UPlayerMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//CanClimbDownLedge();
	//TraceClimbableSurfaces();
	//TraceFromEyeHeight(TraceDistanceFromEyeHeight);
}

void UPlayerMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
		float CapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 0.8f; /* reduce capsule when climbing */
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight);
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		float CapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 0.8; /* restore capsule back to original size when exit climbing */
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight);

		const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
		const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(CleanStandRotation);

		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UPlayerMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	if (IsClimbing())
	{
		PhysClimb(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

float UPlayerMovement::GetMaxSpeed() const
{
	if (IsClimbing())
	{
		return MaxClimbSpeed;
	}
	else
	{
		return Super::GetMaxSpeed();
	}
}

float UPlayerMovement::GetMaxAcceleration() const
{
	if (IsClimbing())
	{
		return MaxClimbAcceleration;
	}
	else
	{
		return Super::GetMaxAcceleration();
	}
}

FVector UPlayerMovement::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	const bool bIsPlayingRMMontage =
		IsFalling() &&
		OwningPlayerAnimInstance &&
		OwningPlayerAnimInstance->IsAnyMontagePlaying();

	if (bIsPlayingRMMontage)
	{
		return RootMotionVelocity;
	}
	else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	}

}

#pragma endregion



#pragma region ClimbCore

void UPlayerMovement::ToggleClimbing(bool bEnableClimbing)
{
	if (bEnableClimbing)
	{
		if (CanStartClimbing())
		{
			/* climb immediatly */
			if (IsFalling())
			{
				StartClimbing();
				return;
			}
			/* play montage */
			PlayClimbMontage(IdleToClimbMontage);

		}
		else if (CanClimbDownLedge())
		{
			PlayClimbMontage(ClimbDownLedgeMontage);
		}
	}
	else
	{
		StopClimbing();
	}
}

bool UPlayerMovement::IsClimbing() const
{
	//if (!(MovementMode == MOVE_Custom)) Debug::Print(TEXT("MovementMode != MOVE_Custom"));
	//if(!(CustomMovementMode == ECustomMovementMode::MOVE_Climb)) Debug::Print(TEXT("CustomMovementMode != ECustomMovementMode::MOVE_Climb"));

	return (MovementMode == MOVE_Custom) && (CustomMovementMode == ECustomMovementMode::MOVE_Climb);
}

FVector UPlayerMovement::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

bool UPlayerMovement::IsClimbButtonVaild()
{
	if (IsClimbing()) return false;

	return CanStartClimbing() || CanClimbDownLedge();
}

// Trace for climbable surfaces, return true if there are valid surfaces
bool UPlayerMovement::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * ClimbCapsuleTraceForwardOffset;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	ClimbableSurfaceTracedResults = DoCapsuleTraceMultiByObject(Start, End/*, true*/);

	return !ClimbableSurfaceTracedResults.IsEmpty();
}

FHitResult UPlayerMovement::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start, End/*, true*/);
}

bool UPlayerMovement::CanStartClimbing()
{
	//if (IsFalling()) return false;

	if (!TraceClimbableSurfaces()) return false;

	if (!TraceFromEyeHeight(TraceDistanceFromEyeHeight).bBlockingHit) return false;

	return true;
}

bool UPlayerMovement::CanClimbDownLedge()
{
	if (IsFalling()) return false;

	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector DownVector = -UpdatedComponent->GetUpVector();

	const FVector WalkableSurfaceTraceStart = ComponentLocation + ComponentForward * ClimbDownWalkableSurfaceTraceOffset;
	const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * ClimbDownTraceDistanceDownload;

	FHitResult WalkableSurfaceHit =
		DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd/*, true*/);

	const FVector LedgeTraceStart = WalkableSurfaceHit.TraceStart + ComponentForward * ClimbDownLedgeTraceOffset;
	const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * ClimbDownTraceDistanceDownload;

	FHitResult LedgeTraceHit =
		DoLineTraceSingleByObject(LedgeTraceStart, LedgeTraceEnd /*,true*/);

	if (WalkableSurfaceHit.bBlockingHit && !LedgeTraceHit.bBlockingHit)
	{
		return true;
	}

	return false;
}

void UPlayerMovement::StartClimbing()
{
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::MOVE_Climb);
}

void UPlayerMovement::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UPlayerMovement::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	/* Process all the climbable surfaces info */
	TraceClimbableSurfaces();

	ProcessClimbableSurafaceInfo();

	/* Check if we should stop climbing */
	if (CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		// Define the max climb speed and acceleration
		CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);

	// Handle climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);

	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	/* Snap movement to climbable surfaces */
	SnapMovementToClimbableSurfaces(deltaTime);

	if (CheckHasReachedLedge())
	{
		PlayClimbMontage(ClimbToTopMontage);
	}

}

void UPlayerMovement::ProcessClimbableSurafaceInfo()
{

	if (ClimbableSurfaceTracedResults.Num() == 0) return;

	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	/** Calculate the average location and normal of the climbable surfaces */
	//AverageClimbableSurfaceLocationAndNormal();

	/** Calculate the nearest location and normal of the climbable surfaces */
	NearesrClimbableSurfaceLocationAndNormal();

	Debug::Print(TEXT("ClimbableSurfaceLocation : ") + CurrentClimbableSurfaceLocation.ToCompactString(), FColor::Cyan, 1);
	Debug::Print(TEXT("ClimbableSurfaceNormal : ") + CurrentClimbableSurfaceNormal.ToCompactString(), FColor::Red, 2);
}

void UPlayerMovement::AverageClimbableSurfaceLocationAndNormal()
{
	for (const FHitResult& TracedHitResult : ClimbableSurfaceTracedResults)
	{
		CurrentClimbableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TracedHitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfaceTracedResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
}

void UPlayerMovement::NearesrClimbableSurfaceLocationAndNormal()
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	float MinDistanceSquared = FLT_MAX;
	FHitResult NearestHitResult;

	for (const FHitResult& TracedHitResult : ClimbableSurfaceTracedResults)
	{
		float DistanceSquared = (ComponentLocation - TracedHitResult.ImpactPoint).SizeSquared();
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			NearestHitResult = TracedHitResult;
		}
	}
	CurrentClimbableSurfaceLocation = NearestHitResult.ImpactPoint;

	FVector SurafaceNormalXY = FVector(NearestHitResult.ImpactNormal.X, NearestHitResult.ImpactNormal.Y, 0.0f).GetSafeNormal();
	if (SurafaceNormalXY != FVector::ZeroVector)
	{
		CurrentClimbableSurfaceNormal = SurafaceNormalXY;
	}
}

bool UPlayerMovement::CheckShouldStopClimbing()
{
	if (ClimbableSurfaceTracedResults.IsEmpty()) return true;

	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);

	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if (DegreeDiff < DegreeDiffToExitClimbing)
	{
		Debug::Print(TEXT("DegreeDiff : ") + FString::SanitizeFloat(DegreeDiff), FColor::Cyan, 1);
		return true;
	}

	//Debug::Print(TEXT("DegreeDiff : ") + FString::SanitizeFloat(DegreeDiff), FColor::Cyan, 1);

	return false;
}

bool UPlayerMovement::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * FloorCheckDistance;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start, End);

	if (PossibleFloorHits.IsEmpty()) return false;

	for (const FHitResult& PossibleFloorHit : PossibleFloorHits)
	{
		const bool bFloorReached =
			FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) &&
			GetUnrotatedClimbVelocity().Z < -10.f;	/* moving down */

		if (bFloorReached) return true;
	}

	return false;
}

FQuat UPlayerMovement::GetClimbRotation(float deltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}

	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat, TargetQuat, deltaTime, 5.f);
}

void UPlayerMovement::SnapMovementToClimbableSurfaces(float deltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface =
		(CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Size();

	UpdatedComponent->MoveComponent(
		SnapVector * deltaTime * MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true);
}

bool UPlayerMovement::CheckHasReachedLedge()
{
	if (CheckHasSomethingAboveHead()) return false;

	FHitResult LedgeHitResult =
		TraceFromEyeHeight(TraceDistanceFromEyeHeight, TraceOffsetFromEyeHeight);

	Debug::Print(TEXT("LedgeHitResult : ") + LedgeHitResult.ImpactPoint.ToCompactString(), FColor::Cyan, 3);

	if (!LedgeHitResult.bBlockingHit)
	{

		const FVector WalkableSurfaceTraceStart = LedgeHitResult.TraceEnd;
		const FVector DownVector = -UpdatedComponent->GetUpVector();
		const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * WalkableSurfaceDownOffset;

		FHitResult WalkableSurfaceHitResult =
			DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd/*,true*/);

		if (WalkableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f)
		{
			return true;
			Debug::Print(TEXT("ReachedLedge "), FColor::Cyan, 4);
		}
	}
	return false;
}

bool UPlayerMovement::CheckHasSomethingAboveHead()
{
	const FVector ForwardOffset = UpdatedComponent->GetForwardVector() * ClimbCapsuleTraceForwardOffset;
	const FVector Start = UpdatedComponent->GetComponentLocation() + ForwardOffset;
	const FVector End = Start + UpdatedComponent->GetUpVector() * 100.f;

	FHitResult AboveHeadHitResult =
		DoLineTraceSingleByObject(Start, End/*,true*/);

	return AboveHeadHitResult.bBlockingHit;
}

void UPlayerMovement::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;
	if (!OwningPlayerAnimInstance) return;
	if (OwningPlayerAnimInstance->IsAnyMontagePlaying()) return;

	OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
}

void UPlayerMovement::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == IdleToClimbMontage || Montage == ClimbDownLedgeMontage)
	{
		StartClimbing();
		//StopMovementImmediately();
	}
	if (Montage == ClimbToTopMontage)
	{
		SetMovementMode(MOVE_Walking);
	}
}

#pragma endregion



#pragma region ClimbTraces
TArray<FHitResult> UPlayerMovement::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistentShapes)
{
	TArray<FHitResult> OutCapsuleTranceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTranceHitResults,
		true
	);

	return OutCapsuleTranceHitResults;
}

FHitResult UPlayerMovement::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistentShapes)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		true
	);

	return OutHit;
}

#pragma endregion