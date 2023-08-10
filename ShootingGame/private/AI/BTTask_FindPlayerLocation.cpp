// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPlayerLocation.h"
#include "AI/EnemyAIController.h"
#include "AIModule/Classes/AIController.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "ShootingGameCharacter.h"

#include "Kismet/GameplayStatics.h"

#include "Interface/IEnemyAIAction.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation()
{
	NodeName = TEXT("Find Player Location");

	// Accept only vector
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPlayerLocation, BlackboardKey));

}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	// Get player 
	//AShootingGameCharacter* const player = Cast<AShootingGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	AEnemyAIController* enemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (nullptr != enemyController) {
		FVector location = enemyController->getTargetActor()->GetActorLocation();
		enemyController->fire(location);
		// Move to the actor (player or player's team member) and stay with a distance is 300
		enemyController->MoveToActor(enemyController->getTargetActor(), 300.0f, false);
	}

	// Signal BehaviorTreeComponent that the task finished with a success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_FindPlayerLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
