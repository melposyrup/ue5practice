// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "HideAndSeekCharacter.generated.h"

class UPlayerMovement;
class UPlayerInteraction;
class AHNSPawnTarget;
class UPlayerAttributes;
class APlayerController;
class UHNSOverlay;
class USoundBase;
class UAudioComponent;

UCLASS(config = Game)
class AHideAndSeekCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHideAndSeekCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void SetOverlappingItem(AHNSPawnTarget* Target);
	void AddScore();
	void PlayInteractSound(bool bPlay);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime)override;

private:
	void Move(const FInputActionValue& Value);
	void MoveOnGround(const FInputActionValue& Value);
	void MoveOnClimb(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnClimbStarted(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void OnSprintStarted(const FInputActionValue& Value);
	void OnSprintCompleted(const FInputActionValue& Value);
	void JumpToStopClimbing(const FInputActionValue& Value);

	void InitializeSlashOverlay();
	void SetHUDScore();
	void ShowGameClearOverlay();
	void UseMouseCursor(bool bUseMouse);


private:
	/** Character components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerMovement, meta = (AllowPrivateAccess = "true"))
	UPlayerMovement* PlayerMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInteraction, meta = (AllowPrivateAccess = "true"))
	UPlayerInteraction* PlayerInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInteraction, meta = (AllowPrivateAccess = "true"))
	AHNSPawnTarget* OverlappingTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerAttributes, meta = (AllowPrivateAccess = "true"))
	UPlayerAttributes* PlayerAttributes;

	/** Overlay */
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UHNSOverlay* Overlay;

	/** Sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* InteractSound;

	UAudioComponent* AudioComponent;

	/** Input */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ClimbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UPlayerMovement* GetPlayerMovemenntComponent() const { return PlayerMovement; }
	FORCEINLINE UPlayerInteraction* GetPlayerInteractionComponent() const { return PlayerInteraction; }
	FORCEINLINE AHNSPawnTarget* GetPlayerOverlappingTarget() const { return OverlappingTarget; }
	FORCEINLINE UPlayerAttributes* GetPlayerAttributes() const { return PlayerAttributes; }
	FORCEINLINE UHNSOverlay* GetPlayerOverlay() const { return Overlay; }
};

