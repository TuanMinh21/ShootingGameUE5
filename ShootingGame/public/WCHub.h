// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WCHub.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SHOOTINGGAME_API UWCHub : public UUserWidget
{
	GENERATED_BODY()

public:

	void setHealthBar(float currentHealth);

	void updateNumberOfMember(bool isTeamA, uint8 number);
	
protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UImage* AimTarget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* TeamAText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* TeamBText;
};
