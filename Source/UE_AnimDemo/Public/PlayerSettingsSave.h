//
//  PlayerSettingsSave.h
//  
//
//  Created by Derrick Auyoung on 21/08/2025.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSettingsSave.generated.h"

UCLASS()
class UE_ANIMDEMO_API UPlayerSettingsSave : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category="Camera")
    float MouseSensitivity = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category="Camera")
    float MouseSmoothing = 5.0f;

    UPROPERTY(BlueprintReadWrite, Category="Camera")
    bool bInvertY = false;
};
