// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimeLineComponent.h"
#include "Constant/TeamType.h"

#include "DoOnce.h"

#include "TimerManager.h"

#include "GameFramework/Character.h"

#include "InputActionValue.h"

#include "ShootingGameCharacter.generated.h"

UCLASS(config=Game)
class AShootingGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

public:

	AShootingGameCharacter();

	bool isAiming;

	UPROPERTY(EditAnyWhere, Category = "Class Type")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY()
	class UWCHub* playerHUD;

	UPROPERTY(EditAnyWhere, Category = "Class Type")
	TSubclassOf<class UUserWidget> TeamWonWidgetClass;

	UPROPERTY()
	class UWCTeamWinner* teamWinnerHUD;

	TeamType GetTeam();

private:

	FTimeline AimingTimeline;

	FTimerHandle FireDelayHandle;

	USkeletalMeshComponent* rifle;

	class AGSTeamDeathMatch* gameState;
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for aiming input */
	void Aim(const FInputActionValue& Value);

	/** Called for aiming input */
	void Fire(const FInputActionValue& Value);

	void StartFireForward();

	void StopFireFoward();

	void delay(float seconds);

	void disableDelay();

	void updateRifleSkeletalMesh();

	/* Aim Curve Float */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Timeline")
	class UCurveFloat* AimingCurve;

	/** Shooting Animation Montage */
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TeamType playerTeam;

	UFUNCTION()
	void AimingUpdate(float Alpha);

	UPROPERTY(EditAnywhere, Category = Health)
	float health = 100.0f;

	UPROPERTY(VisibleAnywhere)
	FDoOnce DoOnce = FDoOnce();

	// Take damage from player
	virtual float TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Call everytime
	virtual void Tick(float DeltaTime) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

