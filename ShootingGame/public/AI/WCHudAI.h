// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WCHudAI.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SHOOTINGGAME_API UWCHudAI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void setAIHealthBar(float currentHealth);

	void setTeamText(bool isTeamA);

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* AIHealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TeamText;
};
