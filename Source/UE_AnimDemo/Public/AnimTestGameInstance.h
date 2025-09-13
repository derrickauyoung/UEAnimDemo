//
//  AnimTestGameInstance.h
//  
//
//  Created by Derrick Auyoung on 21/08/2025.
//
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerSettingsSave.h"
#include "AnimTestGameInstance.generated.h"

UCLASS()
class UE_ANIMDEMO_API UAnimTestGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UPlayerSettingsSave* PlayerSettings;

    virtual void Init() override;

    void SavePlayerSettings();

    void ApplySettingsToCharacter(class AAnimCppChar* Character);
};
