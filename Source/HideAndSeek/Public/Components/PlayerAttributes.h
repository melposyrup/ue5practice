// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttributes.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HIDEANDSEEK_API UPlayerAttributes : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttributes();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddScore();
	bool IsLevelClear();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerAttributes, meta = (AllowPrivateAccess = "true"))
	int32 Score = 0;
	const int32 MaxScore = 20;

public:
	FORCEINLINE int32 GetScore() const { return Score; }
	FORCEINLINE int32 GetMaxScore() const { return MaxScore; }
};
