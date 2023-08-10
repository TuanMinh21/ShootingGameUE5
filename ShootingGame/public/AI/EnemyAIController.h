// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class SHOOTINGGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyAIController();

	void setEnemyAIAction(class IEnemyAIAction* action);

	void fire(FVector location);

	void notifyOwnerDead();

	void notifyTargetActorDead(AActor* actor);

	AActor* getTargetActor();

protected:

	UFUNCTION()
	void onUpdated(AActor* actor, FAIStimulus const stimulus);

	void setupPerceptionSystem();

	bool isMyTeamMember(AActor* actor);

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> behaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBehaviorTreeComponent> behaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBlackboardComponent> blackboardComponent;

	class IEnemyAIAction* enemyAction = nullptr;

	bool isOnwerDead;

	AActor* targetActor;

	class AEnemyAI* owner;
};
