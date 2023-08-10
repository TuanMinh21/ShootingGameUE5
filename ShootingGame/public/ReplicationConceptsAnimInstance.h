// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "ReplicationConceptsAnimInstance.generated.h"

UCLASS(transient, Blueprintable, hideCategories=AnimInstance, BlueprintType)
class UReplicationConceptsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UReplicationConceptsAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Generic")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Pitch;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Aiming")
	bool isAiming;

private:
	class APawn* pawnOwner;

	class AShootingGameCharacter* myPlayer;
};