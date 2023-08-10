// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicationConceptsAnimInstance.h"
#include "ShootingGameCharacter.h"

#include "Engine/GameEngine.h"

UReplicationConceptsAnimInstance::UReplicationConceptsAnimInstance()
{
	Speed = 0.0;
	isAiming = false;
	pawnOwner = NULL;
	myPlayer = NULL;
}

void UReplicationConceptsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (pawnOwner != nullptr) {
		Speed = pawnOwner->GetVelocity().Size();
		
		if (myPlayer != nullptr) {
			this->isAiming = myPlayer->isAiming;
		}

		// Look up, look forward and look down aim offset
		float pitch = pawnOwner->GetBaseAimRotation().Pitch;
		this->Pitch = pitch >= 180.0f ? pitch - 360.0f : pitch;
	}
}

void UReplicationConceptsAnimInstance::NativeInitializeAnimation()
{
	pawnOwner = this->TryGetPawnOwner();
	myPlayer = Cast<AShootingGameCharacter>(pawnOwner);
}