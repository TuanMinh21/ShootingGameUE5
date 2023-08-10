// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TeamType.generated.h"

UENUM(BlueprintType, Category="GameRules")
enum class TeamType : uint8
{
	TeamA UMETA(DisplayName="Team A"),
	TeamB UMETA(DisplayName="Team B")
};
