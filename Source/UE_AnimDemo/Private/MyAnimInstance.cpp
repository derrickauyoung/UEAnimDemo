//
//  MyAnimInstance.cpp
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#include "MyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimCppChar.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimationPoseData.h"
#include "Animation/BlendSpace.h"

UMyAnimInstance::UMyAnimInstance()
{
    CurrentState = ECharacterAnimState::Idle;
}

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    OwningCharacter = Cast<AAnimCppChar>(TryGetPawnOwner());
    
    LocomotionBlendSpace = LoadObject<UBlendSpace>(
        nullptr,
        TEXT("/Game/Animations/IdleWalkRun_BS.IdleWalkRun_BS")
    );

    if (!LocomotionBlendSpace)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load LocomotionBlendSpace at runtime!"));
    }
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    PlayAnimations(DeltaSeconds);
}

void UMyAnimInstance::PlayAnimations(float DeltaSeconds)
{
    if (CurrentState == ECharacterAnimState::None) return;

    switch (CurrentState)
    {
    case ECharacterAnimState::Idle:
        UE_LOG(LogTemp, Warning, TEXT("CharacterAnimState: Idle"));
        if (IdleAnimation && bIsIdle)
        {
            UE_LOG(LogTemp, Warning, TEXT("IdleAnimation && bIsIdle"));
            PlaySlotAnimationAsDynamicMontage(IdleAnimation, FName("DefaultSlot"), 0.25f, 0.25f, 1.f, 1);
        }
        break;

    case ECharacterAnimState::Locomotion:
        UE_LOG(LogTemp, Warning, TEXT("CharacterAnimState: Locomotion"));
        if (LocomotionBlendSpace)
        {
            UE_LOG(LogTemp, Warning, TEXT("LocomotionBlendSpace"));
            // Update blend space pose
            // In pure C++, you can call GetAnimationPose manually if needed,
            // or simply rely on AnimBP nodes to read LocomotionBlendSpaceInput
        }
        break;

    case ECharacterAnimState::Jump:
        UE_LOG(LogTemp, Warning, TEXT("CharacterAnimState: Jump"));
        if (JumpAnimation && bIsJumping)
        {
            UE_LOG(LogTemp, Warning, TEXT("JumpAnimation && bIsJumping"));
            PlaySlotAnimationAsDynamicMontage(JumpAnimation, FName("DefaultSlot"), 0.25f, 0.25f, 1.f, 1);
        }
        break;
    }
}
