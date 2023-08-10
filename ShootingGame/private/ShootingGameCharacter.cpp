// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingGameCharacter.h"

#include "AI/EnemyAIController.h"
#include "AI/EnemyAI.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TimeLineComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameModes/GSTeamDeathMatch.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/UserWidget.h"

#include "WCHub.h"

#include "WCTeamWinner.h"

// AShootingGameCharacter
AShootingGameCharacter::AShootingGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	isAiming = false;

	DoOnce.Reset();
}

void AShootingGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("AimingUpdate"));
	AimingTimeline.AddInterpFloat(AimingCurve, ProgressUpdate, FName("Alpha"));

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Create a widget with crosshair and health bar
	if (IsValid(HUDWidgetClass) && IsValid(TeamWonWidgetClass)) {
		// create player health bar widget
		playerHUD = CreateWidget<UWCHub>(GetWorld(), HUDWidgetClass);
		check(playerHUD);
		playerHUD->AddToPlayerScreen();
		playerHUD->setHealthBar(health);

		// create team winner widget
		teamWinnerHUD = CreateWidget<UWCTeamWinner>(GetWorld(), TeamWonWidgetClass);
		check(teamWinnerHUD);

		// Set player hud for game state update amount of member each team
		gameState = Cast<AGSTeamDeathMatch>(GetWorld()->GetGameState());
		if (IsValid(gameState)) {
			gameState->setPlayerHUD(playerHUD, teamWinnerHUD);
		}
	}

	// Update Rifle subskeletal of Mesh
	updateRifleSkeletalMesh();
}

void AShootingGameCharacter::Tick(float DeltaTime)
{
	AimingTimeline.TickTimeline(DeltaTime);
}

// Input
void AShootingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShootingGameCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShootingGameCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShootingGameCharacter::Aim);

		//Firing - Continuos shooting
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AShootingGameCharacter::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShootingGameCharacter::StartFireForward);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShootingGameCharacter::StopFireFoward);
	}

}

void AShootingGameCharacter::Move(const FInputActionValue& Value)
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

void AShootingGameCharacter::Look(const FInputActionValue& Value)
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

void AShootingGameCharacter::AimingUpdate(float Alpha)
{
	float ArmLenght = FMath::Lerp(400.0f, 170.0f, Alpha);

	GetCameraBoom()->TargetArmLength = ArmLenght;
}

void AShootingGameCharacter::Aim(const FInputActionValue& Value)
{
	isAiming = !isAiming;

	if (isAiming) {
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		AimingTimeline.Play();
	}
	else {
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		AimingTimeline.Reverse();
	}
}

void AShootingGameCharacter::Fire(const FInputActionValue& Value)
{
	if (DoOnce.Execute()) {

		if (nullptr != rifle) {

			// Rifle shoot montage
			PlayAnimMontage(shootMontage, 1.0f, NAME_None);

			// Head of the Rifle Mesh
			FTransform transform = rifle->GetSocketTransform("b_gun_muzzleflash", ERelativeTransformSpace::RTS_World);

			FVector Scale(0.2f, 0.2f, 0.2f);

			// Attaching muzzle flash at the head of the rifle
			UGameplayStatics::SpawnEmitterAttached(
				muzzleFlash,
				rifle,
				"b_gun_muzzleflash",
				transform.GetLocation(),
				FollowCamera->GetComponentRotation(),
				Scale,
				EAttachLocation::KeepWorldPosition,
				true,
				EPSCPoolMethod::AutoRelease,
				true
			);
			
			// Rifle sound effect
			UGameplayStatics::PlaySoundAtLocation(
				this,
				rifleSound,
				GetActorLocation(),
				0.7f
			);

			FHitResult Hit;
			FVector Start = transform.GetLocation();
			FVector End = transform.GetLocation() + UKismetMathLibrary::GetForwardVector(FollowCamera->GetComponentRotation()) * 3000.0f;
			FCollisionQueryParams TraceParams;
			TArray<AActor*> actorsToIgnore;

			//bool isShooted = GetWorld()->LineTraceSingleByChannel(OUT Hit, Start, End, ECC_Visibility, TraceParams);

			bool isShooted = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				Start,
				End,
				20.0f,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				actorsToIgnore,
				EDrawDebugTrace::None,
				Hit,
				true
			);

			if (isShooted) {

				FPointDamageEvent DamageEvent(
					10.0f,
					Hit,
					UKismetMathLibrary::GetForwardVector(FollowCamera->GetComponentRotation()),
					nullptr
				);

				Hit.GetActor()->TakeDamage(
					10.0f,
					DamageEvent,
					GetInstigatorController(),
					this
				);

				// Attaching Troll Death Impact at the destination of the bullet
				FVector scale(1.0f, 1.0f, 1.0f);
				UGameplayStatics::SpawnEmitterAtLocation(
					this,
					trollDeathImpact,
					Hit.ImpactPoint,	// Destination of the bullet
					UKismetMathLibrary::MakeRotFromX(Hit.Normal),
					scale,
					true,
					EPSCPoolMethod::AutoRelease
				);

				// Rifle sound effect
				UGameplayStatics::PlaySoundAtLocation(
					this,
					hitConcreteSound,
					Hit.GetActor()->GetActorLocation(),
					0.35f
				);
			}

			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
		}

		delay(0.08f);
	}
}

void AShootingGameCharacter::StartFireForward()
{
	if (!isAiming) {
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void AShootingGameCharacter::StopFireFoward()
{
	if (!isAiming) {
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void AShootingGameCharacter::delay(float seconds) {
	GetWorld()->GetTimerManager().SetTimer(FireDelayHandle, this, &AShootingGameCharacter::disableDelay, seconds, true);
}

void AShootingGameCharacter::disableDelay() {
	
	if (FireDelayHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(FireDelayHandle);
		DoOnce.Reset();
	}
}

float AShootingGameCharacter::TakeDamage(float damageAmount, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	if (Cast<AShootingGameCharacter>(damageCauser)) {
		// ignore take damage itself
		return damageAmount;
	}
	else if (health == 0) {
		// Notify player was dead
		AEnemyAIController* enemyController = Cast<AEnemyAIController>(eventInstigator);

		if (IsValid(enemyController)) {
			enemyController->notifyTargetActorDead(this);
		}

		return damageAmount;
	}

	float ActualDamage = damageAmount;

	ActualDamage = FMath::Min(health, ActualDamage);

	health -= ActualDamage;

	playerHUD->setHealthBar(health);

	if (health == 0) {

		// Enable simulate physics
		GetMesh()->SetSimulatePhysics(true);

		AEnemyAIController* enemyController = Cast<AEnemyAIController>(eventInstigator);

		if (IsValid(enemyController)) {
			enemyController->notifyTargetActorDead(this);
		}

		// Set player down when its health is over
		GetCharacterMovement()->SetMovementMode(MOVE_None, 0);

		if(IsValid(gameState))
			gameState->notifyTeamMemberChanged(playerTeam == TeamType::TeamA);

	}

	return ActualDamage;
}

void AShootingGameCharacter::updateRifleSkeletalMesh() {

	TArray<USceneComponent*> skeletalMeshComponentArray;
	// Equal GetMesh()
	USkeletalMeshComponent* meshComponent = FindComponentByClass<USkeletalMeshComponent>();

	if (nullptr != meshComponent) {

		meshComponent->GetChildrenComponents(true, skeletalMeshComponentArray);

		for (USceneComponent* childComponent : skeletalMeshComponentArray) {
			if (childComponent->GetFName() == FName("Rifle")) {
				// Cast component
				rifle = Cast<USkeletalMeshComponent>(childComponent);
				break;
			}
		}
	}
}

TeamType AShootingGameCharacter::GetTeam() {
	return playerTeam;
}

/* Display screen for debug

	-- Original debug --
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message MOVE!"));

	-- Debug with input value --
	if (GEngine) {
		FString PlayerInfo = FString::Printf(
			TEXT("Rank: %d - Name: %s - Health: %f"),
			3,
			TEXT("Romero"),
			0.4
		);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hello %s"), *PlayerInfo));
	}
*/