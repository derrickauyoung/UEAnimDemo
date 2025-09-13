//
//  AnimationStateMachine.h
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimationState.h"
#include "AnimationStateMachine.generated.h"

USTRUCT()
struct FAnimationStateData
{
    GENERATED_BODY()

    UPROPERTY()
    UAnimSequence* Animation;
    
    UPROPERTY()
    UBlendSpace* BlendSpace;
    
    bool bLooping;
    float PlayRate;
    float BlendInTime;
    float BlendOutTime;
    
    FAnimationStateData()
        : Animation(nullptr)
        , BlendSpace(nullptr)
        , bLooping(true)
        , PlayRate(1.0f)
        , BlendInTime(0.25f)
        , BlendOutTime(0.25f)
    {}
};

USTRUCT()
struct FStateTransition
{
    GENERATED_BODY()
    
    ECharacterAnimState FromState;
    ECharacterAnimState ToState;
    TFunction<bool()> Condition;
    float TransitionDuration;
    
    FStateTransition()
        : FromState(ECharacterAnimState::Idle)
        , ToState(ECharacterAnimState::Idle)
        , TransitionDuration(0.25f)
    {}
    
    FStateTransition(ECharacterAnimState From, ECharacterAnimState To, float Duration = 0.25f)
        : FromState(From)
        , ToState(To)
        , TransitionDuration(Duration)
    {}
};

UCLASS()
class UE_ANIMDEMO_API UAnimationStateMachine : public UObject
{
    GENERATED_BODY()

public:
    UAnimationStateMachine();
    
    // Initialize the state machine
    void Initialize(USkeletalMeshComponent* InMeshComponent);
    
    // Update the state machine each frame
    void Tick(float DeltaTime);
    
    // Manually force a state change
    void ForceState(ECharacterAnimState NewState);
    
    // Get current state
    ECharacterAnimState GetCurrentState() const { return CurrentState; }
    
    // Register animations for states
    void RegisterStateAnimation(ECharacterAnimState State, UAnimSequence* Animation, bool bLooping = true, float PlayRate = 1.0f);
    void RegisterStateBlendSpace(ECharacterAnimState State, UBlendSpace* BlendSpace, bool bLooping = true, float PlayRate = 1.0f);
    
    // Add transition conditions
    void AddTransition(ECharacterAnimState FromState, ECharacterAnimState ToState, TFunction<bool()> Condition, float Duration = 0.25f);
    
    // Set input parameters for blend spaces
    void SetBlendSpaceInput(float Value) { BlendSpaceInputValue = Value; }

private:
    UPROPERTY()
    USkeletalMeshComponent* MeshComponent;
    
    UPROPERTY()
    TMap<ECharacterAnimState, FAnimationStateData> StateAnimations;
    
    TArray<FStateTransition> Transitions;
    
    ECharacterAnimState CurrentState;
    ECharacterAnimState PreviousState;
    
    float StateTime;
    float TransitionTime;
    float CurrentTransitionDuration;
    bool bIsTransitioning;
    
    float BlendSpaceInputValue;
    
    // Internal methods
    void UpdateTransitions();
    void PlayStateAnimation(ECharacterAnimState State);
    void StartTransition(ECharacterAnimState NewState, float Duration);
    bool CanTransitionTo(ECharacterAnimState NewState) const;
};

