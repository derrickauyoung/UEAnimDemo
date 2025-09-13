//
//  AnimTestGameInstance.cpp
//  
//
//  Created by Derrick Auyoung on 21/08/2025.
//
#include "AnimTestGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "AnimCppChar.h"

void UAnimTestGameInstance::Init()
{
    Super::Init();

    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSettingsSlot"), 0))
    {
        PlayerSettings = Cast<UPlayerSettingsSave>(
            UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettingsSlot"), 0));
    }
    else
    {
        PlayerSettings = Cast<UPlayerSettingsSave>(
            UGameplayStatics::CreateSaveGameObject(UPlayerSettingsSave::StaticClass()));
    }
}

void UAnimTestGameInstance::SavePlayerSettings()
{
    UGameplayStatics::SaveGameToSlot(PlayerSettings, TEXT("PlayerSettingsSlot"), 0);
}

void UAnimTestGameInstance::ApplySettingsToCharacter(AAnimCppChar* Character)
{
    if (!PlayerSettings || !Character) return;

    Character->SetMouseSensitivity(PlayerSettings->MouseSensitivity);
    Character->SetMouseSmoothing(PlayerSettings->MouseSmoothing);
    Character->SetInvertY(PlayerSettings->bInvertY);
}
