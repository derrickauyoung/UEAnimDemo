//
//  MyAnimInstance.h
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimationState.h"
#include "Animation/BlendSpace.h"
#include "MyAnimInstance.generated.h"

UCLASS()
class UE_ANIMDEMO_API UMyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UMyAnimInstance();
    
    /** Called from character to update blend space input */
    void SetLocomotionBlendSpaceInput(float Speed) { LocomotionBlendSpaceInput = Speed; }

    /** Called from character to update jump state */
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetJumping(bool bJumping) { bIsJumping = bJumping; }
    
    // Sets whether the character is in Idle state
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetIdle(bool bIdle) { bIsIdle = bIdle; };

    /** Called from character to set current state */
    void SetCurrentAnimState(ECharacterAnimState NewState) { CurrentState = NewState; }

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    
    // Current state for the state machine
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    ECharacterAnimState CurrentState;
    
    UBlendSpace* LocomotionBlendSpace;
    
    UPROPERTY(BlueprintReadOnly)
    float LocomotionBlendSpaceInput;
    
    /** Is character jumping */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsJumping;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsIdle = true;

    /** Animation assets */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimSequence* IdleAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimSequence* JumpAnimation;
    
    /** Update animations each tick */
    void PlayAnimations(float DeltaSeconds);

private:
    class AAnimCppChar* OwningCharacter;
    void UpdateAnimationState(float DeltaTime);

};
