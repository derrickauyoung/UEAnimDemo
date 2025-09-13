//
//  AnimationStateMachine.cpp
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#include "AnimationStateMachine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpace1D.h"
#include "Animation/AnimInstance.h"

UAnimationStateMachine::UAnimationStateMachine()
{
    CurrentState = ECharacterAnimState::Idle;
    PreviousState = ECharacterAnimState::Idle;
    StateTime = 0.0f;
    TransitionTime = 0.0f;
    CurrentTransitionDuration = 0.0f;
    bIsTransitioning = false;
    BlendSpaceInputValue = 0.0f;
    MeshComponent = nullptr;
}

void UAnimationStateMachine::Initialize(USkeletalMeshComponent* InMeshComponent)
{
    MeshComponent = InMeshComponent;
    
    // Set initial state
    if (MeshComponent)
    {
        PlayStateAnimation(CurrentState);
    }
}

void UAnimationStateMachine::Tick(float DeltaTime)
{
    if (!MeshComponent)
        return;
    
    StateTime += DeltaTime;
    
    // Update transitions if we're currently transitioning
    if (bIsTransitioning)
    {
        TransitionTime += DeltaTime;
        
        float Alpha = FMath::Clamp(TransitionTime / CurrentTransitionDuration, 0.0f, 1.0f);
        
        // Update blend weights or handle transition logic here
        // For simplicity, we'll just check if transition is complete
        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
            TransitionTime = 0.0f;
            PreviousState = CurrentState;
        }
    }
    
    // Check for state transitions
    if (!bIsTransitioning)
    {
        UpdateTransitions();
    }
    
    // Update blend space inputs if current state uses one
    if (StateAnimations.Contains(CurrentState))
    {
        FAnimationStateData& StateData = StateAnimations[CurrentState];
        if (StateData.BlendSpace && MeshComponent->GetAnimInstance())
        {
            // Update blend space parameter - you'd typically expose this as a function parameter
            // or get it from character movement component
            // This is just an example of how you'd set blend space inputs
        }
    }
}

void UAnimationStateMachine::UpdateTransitions()
{
    // Check all registered transitions
    for (const FStateTransition& Transition : Transitions)
    {
        if (Transition.FromState == CurrentState && Transition.Condition && Transition.Condition())
        {
            StartTransition(Transition.ToState, Transition.TransitionDuration);
            break; // Take the first valid transition
        }
    }
}

void UAnimationStateMachine::StartTransition(ECharacterAnimState NewState, float Duration)
{
    if (NewState == CurrentState)
        return;
    
    PreviousState = CurrentState;
    CurrentState = NewState;
    CurrentTransitionDuration = Duration;
    TransitionTime = 0.0f;
    bIsTransitioning = true;
    StateTime = 0.0f;
    
    PlayStateAnimation(NewState);
}

void UAnimationStateMachine::PlayStateAnimation(ECharacterAnimState State)
{
    if (!MeshComponent || !MeshComponent->GetAnimInstance())
        return;
    
    if (!StateAnimations.Contains(State))
        return;
    
    const FAnimationStateData& StateData = StateAnimations[State];
    UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();
    
    if (StateData.Animation)
    {
        // Stop any currently playing montages
        AnimInstance->StopAllMontages(StateData.BlendOutTime);
        
        // Play the animation sequence
        // Note: For a full implementation, you'd want to create and play montages
        // or use a custom AnimInstance that handles state-based playback
        
        // This is a simplified approach - in a real implementation you'd typically
        // create montages from sequences or use a custom AnimInstance
    }
    else if (StateData.BlendSpace)
    {
        // Handle blend space playback
        // This would require custom AnimInstance integration
    }
}

void UAnimationStateMachine::ForceState(ECharacterAnimState NewState)
{
    if (CanTransitionTo(NewState))
    {
        StartTransition(NewState, 0.1f); // Quick transition for forced states
    }
}

bool UAnimationStateMachine::CanTransitionTo(ECharacterAnimState NewState) const
{
    // Add any global transition rules here
    // For example, you might not allow transitions while already transitioning
    // or have certain states that can't be interrupted
    
    if (bIsTransitioning)
        return false;
    
    return true;
}

void UAnimationStateMachine::RegisterStateAnimation(ECharacterAnimState State, UAnimSequence* Animation, bool bLooping, float PlayRate)
{
    FAnimationStateData StateData;
    StateData.Animation = Animation;
    StateData.bLooping = bLooping;
    StateData.PlayRate = PlayRate;
    
    StateAnimations.Add(State, StateData);
}

void UAnimationStateMachine::RegisterStateBlendSpace(ECharacterAnimState State, UBlendSpace* BlendSpace, bool bLooping, float PlayRate)
{
    FAnimationStateData StateData;
    StateData.BlendSpace = BlendSpace;
    StateData.bLooping = bLooping;
    StateData.PlayRate = PlayRate;
    
    StateAnimations.Add(State, StateData);
}

void UAnimationStateMachine::AddTransition(ECharacterAnimState FromState, ECharacterAnimState ToState, TFunction<bool()> Condition, float Duration)
{
    FStateTransition Transition(FromState, ToState, Duration);
    Transition.Condition = Condition;
    Transitions.Add(Transition);
}
