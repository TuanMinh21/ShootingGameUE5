// Fill out your copyright notice in the Description page of Project Settings.


#include "WCHub.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWCHub::setHealthBar(float currentHealth)
{
	if (IsValid(HealthBar)) {
		HealthBar->SetPercent(currentHealth / 100.0f);
	}
}

void UWCHub::updateNumberOfMember(bool isTeamA, uint8 number) {
	if (isTeamA && IsValid(TeamAText)) {
		TeamAText->SetText(FText::FromString(FString::Printf(TEXT("Team A: %d"), number)));
	}
	else if (IsValid(TeamBText)) {
		TeamBText->SetText(FText::FromString(FString::Printf(TEXT("Team B: %d"), number)));
	}
}
