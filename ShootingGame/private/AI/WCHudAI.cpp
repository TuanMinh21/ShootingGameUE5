// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/WCHudAI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "Styling/SlateColor.h"

#include "Math/Color.h"

void UWCHudAI::setAIHealthBar(float currentHealth)
{
	if (IsValid(AIHealthBar)) {
		AIHealthBar->SetPercent(currentHealth / 100.0f);
	}
}

void UWCHudAI::setTeamText(bool isTeamA) {
	if (isTeamA && IsValid(TeamText)) {
		TeamText->SetText(FText::FromString(TEXT("Team A")));
		// Set Text Color
		FLinearColor linearColor(0.0f, 0.5f, 1.0f, 1.0f);
		FSlateColor color(linearColor);
		TeamText->SetColorAndOpacity(color);
	}
	else if (IsValid(TeamText)) {
		TeamText->SetText(FText::FromString(TEXT("Team B")));
		// Set Text Color
		FLinearColor linearColor(1.0f, 0.017f, 0.0f, 1.0f);
		FSlateColor color(linearColor);
		TeamText->SetColorAndOpacity(color);
	}
}
