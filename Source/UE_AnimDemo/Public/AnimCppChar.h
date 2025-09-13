//
//  AnimCppChar.h
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnimationStateMachine.h"
#include "InputActionValue.h"          // For FInputActionValue
#include "InputMappingContext.h"       // For UInputMappingContext
#include "PlayerSettingsWidget.h"      // For UPlayerSettingsWidget
#include "MyAnimInstance.h"
#include "AnimCppChar.generated.h"

UCLASS()
class UE_ANIMDEMO_API AAnimCppChar : public ACharacter
{
    GENERATED_BODY()

public:
    AAnimCppChar();
    
    FORCEINLINE UAnimationStateMachine* GetAnimStateMachine() const { return AnimStateMachine; }

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
    
    UFUNCTION()
    void Move(const FInputActionValue& Value);

    UFUNCTION()
    void Turn(const FInputActionValue& Value);
    
    UFUNCTION()
    void LookUp(const FInputActionValue& Value);
    
    UFUNCTION()
    void ToggleSettingsMenu();
      
    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    class UInputAction* IA_Jump;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    class UInputAction* IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    class UInputAction* IA_Turn;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    class UInputAction* IA_LookUp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    class UInputAction* IA_ToggleSettings;
    
    UFUNCTION(BlueprintCallable, Category="Settings")
    void SetMouseSensitivity(float NewSens);
    
    UFUNCTION(BlueprintCallable, Category="Settings")
    void SetMouseSmoothing(float NewSmooth);
    
    UFUNCTION(BlueprintCallable, Category="Settings")
    void SetInvertY(bool bNewInvert);
    
    UFUNCTION(BlueprintCallable, Category="Settings")
    void SavePlayerSettings();

    UFUNCTION(BlueprintCallable, Category="Settings")
    void LoadPlayerSettings();
    
    UFUNCTION(BlueprintCallable, Category="Settings")
    void HideSettingsWidget();

    UFUNCTION(BlueprintCallable, Category="Settings")
    void ShowSettingsWidget();
    
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    class UCameraComponent* FollowCamera;
    
    // Animation assets - these would be set in constructor or loaded
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Assets")
    UAnimSequence* IdleAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Assets")
    UAnimSequence* WalkAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Assets")
    UAnimSequence* RunAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Assets")
    UAnimSequence* JumpAnimation;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Assets")
    UBlendSpace* MovementBlendSpace;
    
    /** AnimInstance reference */
    UPROPERTY(Transient)
    UMyAnimInstance* OwningAnimInstance;
    
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    // Widget class to spawn (set in Blueprint)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
    TSubclassOf<UPlayerSettingsWidget> SettingsWidgetClass;

    UPROPERTY()
    UPlayerSettingsWidget* SettingsWidget;
    
    UPROPERTY()
    UAnimationStateMachine* AnimStateMachine;
    
private:
    /** Current state */
    ECharacterAnimState CurrentAnimState;

    /** Current blend space input (speed) */
    float CurrentBlendSpaceInput;
    
    void SetupAnimationStateMachine();
    void UpdateAnimationInputs();
    void UpdateAnimationState(float DelaTime);
    
    // Helper functions for transition conditions
    bool ShouldWalk() const;
    bool ShouldRun() const;
    bool ShouldJump() const;
    bool ShouldIdle() const;
    
    float MouseSensitivity = 1.0f;
    float MouseSmoothing = 5.0f; // higher = smoother, slower response
    bool bInvertY = false;
    
    float SmoothedYaw = 0.f;
    float SmoothedPitch = 0.f;
    
    bool bIsToggling = false;
};

