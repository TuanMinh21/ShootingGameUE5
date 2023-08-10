// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Constant/TeamType.h"

#include "AI/EnemyAIController.h"

#include "DoOnce.h"

#include "GameFramework/Character.h"

#include "Interface/IEnemyAIAction.h"

#include "EnemyAI.generated.h"

UCLASS()
class SHOOTINGGAME_API AEnemyAI : public ACharacter, public IEnemyAIAction
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAI();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// From IEnemyAIAction
	virtual void EnemyAIFire(FVector location) override;

	void delay(float seconds);

	void disableDelay();

	void setEnemyRotation(FVector location);

	TeamType GetTeam();

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* healthBarComponent;

	UPROPERTY()
	class UWCHudAI* enemyHUD;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Take damage from player
	virtual float TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser) override;

	void updateRifleSkeletalMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UAnimMontage* shootMontage;

	/** Apply Emitter Attached */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EmitterTemplate)
	class UParticleSystem* muzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EmitterTemplate)
	class UParticleSystem* trollDeathImpact;

	/** MetaSound Effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundEffect)
	class USoundBase* rifleSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundEffect)
	class USoundBase* hitConcreteSound;

	UPROPERTY(EditAnywhere, Category = Health)
	float health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TeamType AITeam;

private:

	USkeletalMeshComponent* rifle;

	FDoOnce DoOnce = FDoOnce();

	FTimerHandle FireDelayHandle;

	AEnemyAIController* ownAIController;

	class AGSTeamDeathMatch* gameState;
};
