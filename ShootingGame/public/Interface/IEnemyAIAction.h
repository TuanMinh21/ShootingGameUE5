// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SHOOTINGGAME_API IEnemyAIAction
{
public:
	
	virtual ~IEnemyAIAction();

	virtual void EnemyAIFire(FVector location) = 0;

};
