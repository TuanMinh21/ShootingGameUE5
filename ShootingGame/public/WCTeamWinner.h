// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WCTeamWinner.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SHOOTINGGAME_API UWCTeamWinner : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void setTeamWonText(bool isTeamAWon);

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TeamWonText;
};
