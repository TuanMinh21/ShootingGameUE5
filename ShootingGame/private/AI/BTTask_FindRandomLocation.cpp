// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindRandomLocation.h"
#include "AIModule/Classes/AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");

	// Accept only vector
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation location;

	// Get AI Pawn
	AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* AIPawn = AIController->GetPawn();

	// Get Pawn Origin
	const FVector Origin = AIPawn->GetActorLocation();

	// Obtain Navigation system and find a random location
	const UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (IsValid(navSystem) && navSystem->GetRandomPointInNavigableRadius(Origin, searchRadius, location)) {
		AIController->GetBlackboardComponent()
					->SetValueAsVector (
						BlackboardKey.SelectedKeyName,
						location.Location
					);
	}

	// Signal BehaviorTreeComponent that the task finished with a success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_FindRandomLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
