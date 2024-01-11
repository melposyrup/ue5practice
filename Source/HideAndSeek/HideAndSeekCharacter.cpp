// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideAndSeekCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PlayerMovement.h"	/* CustomMovementComponent */
#include "Components/PlayerInteraction.h" /* Interaction component */
#include "Components/PlayerAttributes.h" /* Player Attributes */
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DebugHelper.h"
#include "HNSPawnTarget.h"
#include "HUD/HNSHUD.h"
#include "HUD/HNSOverlay.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

//////////////////////////////////////////////////////////////////////////
// AHideAndSeekCharacter

AHideAndSeekCharacter::AHideAndSeekCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovement>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 640.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/** Custormize Actor Component */
	PlayerMovement = Cast<UPlayerMovement>(GetCharacterMovement());
	PlayerInteraction = CreateDefaultSubobject<UPlayerInteraction>(TEXT("PlayerInteraction"));
	PlayerAttributes = CreateDefaultSubobject<UPlayerAttributes>(TEXT("PlayerAttributes"));

}

void AHideAndSeekCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitializeSlashOverlay();

	//Add Input Mapping Context
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (!Overlay) return;
	Overlay->PlayAnimation(Overlay->GetGetIntoGameScene());

	AudioComponent = UGameplayStatics::SpawnSoundAttached(
		InteractSound,
		GetRootComponent(),
		NAME_None,
		GetActorLocation(),
		EAttachLocation::KeepRelativeOffset
	);

	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}

void AHideAndSeekCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** show ClimbTutorial overlay */
	if (!Overlay) return;
	if (PlayerMovement->IsClimbButtonVaild()) {
		Overlay->ShowOverlayBox(Overlay->GetClimbTutorialBox(), true);
	}
	else
	{
		Overlay->ShowOverlayBox(Overlay->GetClimbTutorialBox(), false);
	}

	Overlay->ShowOverlayBox(Overlay->GetJumpOffTutorialBox(), PlayerMovement->IsClimbing());

	/** show Interacr(triggered by raycast) overlay */
	AActor* HitActor = PlayerInteraction->Raycast().GetActor();
	if (HitActor && HitActor->Implements<UHNSInteractInterface>())
	{
		Overlay->ShowOverlayBox(Overlay->GetInteractTutorialBox(), true);
	}
	else
	{
		Overlay->ShowOverlayBox(Overlay->GetInteractTutorialBox(), false);
	}
}

#pragma region Input

void AHideAndSeekCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHideAndSeekCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHideAndSeekCharacter::Look);

		/** Custormize */
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &AHideAndSeekCharacter::OnClimbStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHideAndSeekCharacter::JumpToStopClimbing);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHideAndSeekCharacter::Interact);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHideAndSeekCharacter::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHideAndSeekCharacter::OnSprintCompleted);
	}

}

void AHideAndSeekCharacter::Move(const FInputActionValue& Value)
{
	if (!PlayerMovement) return;

	if (PlayerMovement->IsClimbing())
	{
		MoveOnClimb(Value);
	}
	else
	{
		MoveOnGround(Value);
	}
}

void AHideAndSeekCharacter::MoveOnGround(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AHideAndSeekCharacter::MoveOnClimb(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// get forward vector
		const FVector ForwardDirection = FVector::CrossProduct(
			-PlayerMovement->GetClimbableSurfaceNormal(),
			GetActorRightVector()
		);

		// get right vector 
		const FVector RightDirection = FVector::CrossProduct(
			-PlayerMovement->GetClimbableSurfaceNormal(),
			-GetActorUpVector()
		);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AHideAndSeekCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHideAndSeekCharacter::OnClimbStarted(const FInputActionValue& Value)
{
	//Debug::Print("OnClimbStarted");

	if (!PlayerMovement) return;

	if (!PlayerMovement->IsClimbing())
	{
		PlayerMovement->ToggleClimbing(true);
	}
}

void AHideAndSeekCharacter::Interact(const FInputActionValue& Value)
{
	if (!PlayerInteraction) return;

	PlayerInteraction->Interact(OverlappingTarget);
}

void AHideAndSeekCharacter::SetOverlappingItem(AHNSPawnTarget* Target)
{
	OverlappingTarget = Target;
}

void AHideAndSeekCharacter::OnSprintStarted(const FInputActionValue& Value)
{
	if (!PlayerMovement) return;
	if (PlayerMovement->MovementMode != MOVE_Walking) return;

	PlayerMovement->MaxWalkSpeed = 600.f;

	if (!Overlay) return;
	Overlay->ShowOverlayBox(Overlay->GetSprintTutorialBox(), false);
}

void AHideAndSeekCharacter::OnSprintCompleted(const FInputActionValue& Value)
{
	if (!PlayerMovement) return;

	PlayerMovement->MaxWalkSpeed = 200.f;

	if (!Overlay) return;
	Overlay->ShowOverlayBox(Overlay->GetSprintTutorialBox(), true);
}

void AHideAndSeekCharacter::JumpToStopClimbing(const FInputActionValue& Value)
{
	if (!PlayerMovement) return;

	if (PlayerMovement->IsClimbing())
	{
		PlayerMovement->ToggleClimbing(false);
	}
}

#pragma endregion


#pragma region Process Score and UI

void AHideAndSeekCharacter::AddScore()
{
	if (!PlayerAttributes) return;

	PlayerAttributes->AddScore();
	SetHUDScore();

	if (PlayerAttributes->IsLevelClear())
	{
		ShowGameClearOverlay();
		Debug::Print("Level Clear");
	}
}

void AHideAndSeekCharacter::InitializeSlashOverlay()
{
	PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	AHNSHUD* HNSHUD = Cast<AHNSHUD>(PlayerController->GetHUD());
	if (!HNSHUD) return;

	Overlay = HNSHUD->GetHNSOverlay();
	if (!Overlay) return;

	SetHUDScore();

	UseMouseCursor(false);

	Overlay->ShowOverlayBox(Overlay->GetSprintTutorialBox(), true);
	Overlay->ShowOverlayBox(Overlay->GetClimbTutorialBox(), false);
	Overlay->ShowOverlayBox(Overlay->GetJumpOffTutorialBox(), false);
	Overlay->ShowOverlayBox(Overlay->GetInteractTutorialBox(), false);
	Overlay->ShowOverlayBox(Overlay->GetGameClearBox(), false);

}

void AHideAndSeekCharacter::SetHUDScore()
{
	if (!PlayerAttributes) return;
	if (!Overlay) return;

	Overlay->SetScoreText(
		PlayerAttributes->GetScore(),
		PlayerAttributes->GetMaxScore()
	);
}

void AHideAndSeekCharacter::ShowGameClearOverlay()
{
	Overlay->ShowOverlayBox(Overlay->GetGameClearBox(), true);
	UseMouseCursor(true);

	Overlay->PlayAnimation(Overlay->GetGameClearFadeIn());
}

void AHideAndSeekCharacter::UseMouseCursor(bool bUseMouse)
{
	if (!PlayerController) return;

	if (bUseMouse)
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetShowMouseCursor(true);
	}
	else/** use keyboard */
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->ResetIgnoreLookInput();
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

}

#pragma endregion


#pragma region Sound

void AHideAndSeekCharacter::PlayInteractSound(bool bPlay)
{
	AudioComponent = UGameplayStatics::SpawnSoundAttached(
		InteractSound,
		GetRootComponent(),
		NAME_None,
		GetActorLocation(),
		EAttachLocation::KeepRelativeOffset
	);

	if (!AudioComponent) return;

	if (bPlay)
	{
		AudioComponent->Play(0.f);
	}
	else
	{
		AudioComponent->Stop();
	}
}

#pragma endregion