// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAI.h"
#include "AI/WCHudAI.h"

#include "Components/WidgetComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/EngineTypes.h"

#include "GameModes/GSTeamDeathMatch.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar"));

	healthBarComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	ownAIController = Cast<AEnemyAIController>(GetController());

	if (IsValid(ownAIController)) {
		ownAIController->setEnemyAIAction(this);
	}

	// Update Rifle subskeletal of Mesh
	updateRifleSkeletalMesh();

	DoOnce.Reset();

	enemyHUD = Cast<UWCHudAI>(healthBarComponent->GetUserWidgetObject());
	if (IsValid(enemyHUD)) {
		enemyHUD->setAIHealthBar(health);
		enemyHUD->setTeamText(AITeam == TeamType::TeamA);
	}

	// Cast to Game state death match
	gameState = Cast<AGSTeamDeathMatch>(GetWorld()->GetGameState());
}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector cameraLocation = cameraManager->GetCameraLocation();

	// Health bar rotate follow camera
	FRotator healthBarRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), cameraLocation);
	healthBarComponent->SetWorldRotation(healthBarRotator);
}

// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyAI::EnemyAIFire(FVector location)
{

	setEnemyRotation(location);

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
				GetMesh()->GetComponentRotation(),
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
			FVector End = transform.GetLocation() + UKismetMathLibrary::GetRightVector(GetMesh()->GetComponentRotation()) * 3000.0f;
			FCollisionQueryParams TraceParams;
			TArray<AActor*> actorsToIgnore;

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

				FPointDamageEvent DamageEvent (
					10.0f,
					Hit,
					UKismetMathLibrary::GetForwardVector(GetMesh()->GetComponentRotation()),
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

		}
		delay(0.2f);
	}
	
}


void AEnemyAI::delay(float seconds)
{
	GetWorld()->GetTimerManager().SetTimer(FireDelayHandle, this, &AEnemyAI::disableDelay, seconds, true);
}

void AEnemyAI::disableDelay()
{
	if (FireDelayHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(FireDelayHandle);
		DoOnce.Reset();
	}
}

void AEnemyAI::setEnemyRotation(FVector location)
{
	// Focus on player while shooting
	FRotator playerRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), location);

	// Giving a nice smooth rotation
	FRotator newRotator = FMath::RInterpTo(GetActorRotation(), playerRotator, GetWorld()->DeltaTimeSeconds, 20.0f);

	// Keep Roll (X) and Pitch (Y) while rotating
	newRotator.Roll = GetActorRotation().Roll;
	newRotator.Pitch = GetActorRotation().Pitch;

	SetActorRotation(newRotator, ETeleportType::None);
}

float AEnemyAI::TakeDamage(float damageAmount, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	if (health == 0) {
		AEnemyAIController* enemyController = Cast<AEnemyAIController>(eventInstigator);

		if (IsValid(enemyController)) {
			enemyController->notifyTargetActorDead(this);
		}

		return damageAmount;
	}

	float ActualDamage = damageAmount;

	ActualDamage = FMath::Min(health, ActualDamage);

	health -= ActualDamage;

	enemyHUD->setAIHealthBar(health);

	if (health == 0) {

		// Enable simulate physics
		GetMesh()->SetSimulatePhysics(true);

		// Set enemy down when its health is over
		GetCharacterMovement()->SetMovementMode(MOVE_None, 0);

		// Notify to onwer controller
		ownAIController->notifyOwnerDead();

		// Notify to enemy of AI enemy
		AEnemyAIController* enemyController = Cast<AEnemyAIController>(eventInstigator);

		if (IsValid(enemyController)) {
			enemyController->notifyTargetActorDead(this);
		}

		// Hide health bar
		healthBarComponent->SetVisibility(false);

		if (IsValid(gameState)) {
			gameState->notifyTeamMemberChanged(AITeam == TeamType::TeamA);
		}
	}

	return ActualDamage;
}

void AEnemyAI::updateRifleSkeletalMesh()
{
	TArray<USceneComponent*> skeletalMeshComponentArray;
	USkeletalMeshComponent* meshComponent = GetMesh();

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

TeamType AEnemyAI::GetTeam() {
	return AITeam;
}
