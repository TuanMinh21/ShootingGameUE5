// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "AI/EnemyAI.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Interface/IEnemyAIAction.h"

#include "ShootingGameCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	isOnwerDead = false;
	targetActor = false;
	behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	setupPerceptionSystem();
}

void AEnemyAIController::setEnemyAIAction(IEnemyAIAction* action)
{
	enemyAction = action;
}

void AEnemyAIController::fire(FVector location)
{
	if (nullptr != enemyAction) {
		enemyAction->EnemyAIFire(location);
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(behaviorTree.Get())) {
		RunBehaviorTree(behaviorTree.Get());
		behaviorTreeComponent->StartTree(*behaviorTree.Get());
		owner = Cast<AEnemyAI>(GetPawn());
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(behaviorTree.Get())) {
		Blackboard->InitializeBlackboard(*behaviorTree.Get()->BlackboardAsset.Get());
	}
}

void AEnemyAIController::onUpdated(AActor* actor, FAIStimulus const stimulus)
{
	// Enemy still alive and see the player
	bool isDetected = !isOnwerDead && stimulus.WasSuccessfullySensed();

	if (!isMyTeamMember(actor)) {
		targetActor = actor;
		blackboardComponent.Get()->SetValueAsBool("CanSeePlayer", isDetected); // Can be true or false
	}
}

void AEnemyAIController::notifyOwnerDead()
{
	// Cannot see any more because itself is dead
	isOnwerDead = true;
	blackboardComponent.Get()->SetValueAsBool("CanSeePlayer", false);
}

void AEnemyAIController::notifyTargetActorDead(AActor* actor) {
	if (targetActor == actor) {
		blackboardComponent.Get()->SetValueAsBool("CanSeePlayer", false);
	}
}

AActor* AEnemyAIController::getTargetActor() {
	return targetActor;
}

bool AEnemyAIController::isMyTeamMember(AActor* actor) {

	AEnemyAI* enemy = Cast<AEnemyAI>(actor);

	if (nullptr != enemy) {
		return owner->GetTeam() == enemy->GetTeam();
	}
	else {
		AShootingGameCharacter* player = Cast<AShootingGameCharacter>(actor);

		return owner->GetTeam() == player->GetTeam();
	}
}

void AEnemyAIController::setupPerceptionSystem()
{

	// create and initialise sight configuration object
	UAISenseConfig_Sight* sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	
	if (sight_config)
	{
		sight_config->SightRadius = 1200.0f;
		sight_config->LoseSightRadius = sight_config->SightRadius + 25.0f;
		sight_config->PeripheralVisionAngleDegrees = 75.0f;
		sight_config->SetMaxAge(3.0f);
		sight_config->AutoSuccessRangeFromLastSeenLocation = 1220.0f;
		sight_config->DetectionByAffiliation.bDetectEnemies = true;
		sight_config->DetectionByAffiliation.bDetectFriendlies = true;
		sight_config->DetectionByAffiliation.bDetectNeutrals = true;

		// add sight configuration component to perception component
		GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::onUpdated);
		GetPerceptionComponent()->ConfigureSense(*sight_config);
	}
}