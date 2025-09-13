// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AnimDemoGameMode.generated.h"

UCLASS()
class UE_ANIMDEMO_API AAnimDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    // Make sure this exactly matches the cpp constructor
    AAnimDemoGameMode();
};
