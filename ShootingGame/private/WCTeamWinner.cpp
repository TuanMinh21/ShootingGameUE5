// Fill out your copyright notice in the Description page of Project Settings.


#include "WCTeamWinner.h"

#include "Components/TextBlock.h"

void UWCTeamWinner::setTeamWonText(bool isTeamAWon)
{
	if (IsValid(TeamWonText)) {
		if (isTeamAWon)
		{
			TeamWonText->SetText(FText::FromString("TEAM A WON"));
		}
		else {
			TeamWonText->SetText(FText::FromString("TEAM B WON"));
		}
	}
}
