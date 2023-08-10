// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GSTeamDeathMatch.h"

#include "WCHub.h"
#include "WCTeamWinner.h"

AGSTeamDeathMatch::AGSTeamDeathMatch() {
	numberOfTeamA = 6;
	numberOfTeamB = 6;	
}

void AGSTeamDeathMatch::setPlayerHUD(UWCHub* playerHud, UWCTeamWinner* teamWinnerHud) {
	playerHUD = playerHud;
	teamWinnerHUD = teamWinnerHud;
	// set default value for each team
	notifyTeamMemberChanged(true);
	notifyTeamMemberChanged(false);
}

void AGSTeamDeathMatch::notifyTeamMemberChanged(bool isTeamA) {
	if (nullptr != playerHUD) {
		uint8 number = isTeamA ? --numberOfTeamA : --numberOfTeamB;
 		playerHUD->updateNumberOfMember(isTeamA, number);
	}

	if (numberOfTeamA == 0 || numberOfTeamB == 0) {

		// Set team won and add to player screen
		teamWinnerHUD->setTeamWonText(numberOfTeamA != 0);
		teamWinnerHUD->AddToViewport();
		
		// Game over (pause game)
		APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (MyPlayer != NULL)
		{
			MyPlayer->SetPause(true);
		}
	}
}