// Fill out your copyright notice in the Description page of Project Settings.


#include "HNSPawnTarget.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HideAndSeek/DebugHelper.h"
#include "HideAndSeek/HideAndSeekCharacter.h"
#include "HUD/HNSOverlay.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHNSPawnTarget::AHNSPawnTarget()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(250.f);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
}

void AHNSPawnTarget::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AHNSPawnTarget::OnSphereOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AHNSPawnTarget::OnSphereEndOverlap);

}

void AHNSPawnTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHNSPawnTarget::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHNSPawnTarget::Interact_Implementation(AHideAndSeekCharacter* Player)
{
	Debug::Print(TEXT("Interact_Implementation"));

	//call Player->AddScore()
	Player->AddScore();
	Player->PlayInteractSound(true);

	//destory, may be some animation effects
	Destroy();
}

void AHNSPawnTarget::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHideAndSeekCharacter* Player = Cast<AHideAndSeekCharacter>(OtherActor);
	if (!Player) return;
	Player->SetOverlappingItem(this);

	UHNSOverlay* Overlay = Player->GetPlayerOverlay();
	if (!Overlay) return;
	Overlay->ShowOverlayBox(Overlay->GetInteractTutorialBox(), true);

}

void AHNSPawnTarget::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AHideAndSeekCharacter* Player = Cast<AHideAndSeekCharacter>(OtherActor);
	if (!Player) return;
	Player->SetOverlappingItem(nullptr);

	UHNSOverlay* Overlay = Player->GetPlayerOverlay();
	if (!Overlay) return;
	Overlay->ShowOverlayBox(Overlay->GetInteractTutorialBox(), false);
}
