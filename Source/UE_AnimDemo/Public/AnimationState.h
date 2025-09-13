//
//  AnimationState.h
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimationState.generated.h"

UENUM(BlueprintType)
enum class ECharacterAnimState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Walk            UMETA(DisplayName = "Walk"),
    Run             UMETA(DisplayName = "Run"),
    Jump            UMETA(DisplayName = "Jump"),
    Locomotion      UMETA(DisplayName = "Locomotion"),
    None            UMETA(DisplayName = "None"),
};

// Forward declarations
class UAnimSequence;
class UBlendSpace;
class USkeletalMeshComponent;

