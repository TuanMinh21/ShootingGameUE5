// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GSTeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AGSTeamDeathMatch : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGSTeamDeathMatch();

	virtual ~AGSTeamDeathMatch() = default;

	void setPlayerHUD(class UWCHub* playerHud, class UWCTeamWinner* teamWinnerHud);

	void notifyTeamMemberChanged(bool isTeamA);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Team Alive")
	uint8 numberOfTeamA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team Alive")
	uint8 numberOfTeamB;

private:
	class UWCHub* playerHUD;

	class UWCTeamWinner* teamWinnerHUD;
};
