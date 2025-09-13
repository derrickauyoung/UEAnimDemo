// AnimTestCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"          // For FInputActionValue
#include "InputMappingContext.h"       // For UInputMappingContext
#include "PlayerSettingsWidget.h"      // For UPlayerSettingsWidget
#include "AnimTestCharacter.generated.h"

UCLASS()
class UE_ANIMDEMO_API AAnimTestCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAnimTestCharacter();

protected:
    virtual void BeginPlay() override;
    
    // Widget class to spawn (set in Blueprint)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
    TSubclassOf<UPlayerSettingsWidget> SettingsWidgetClass;

    UPROPERTY()
    UPlayerSettingsWidget* SettingsWidget;
    
private:
    float MouseSensitivity = 1.0f;
    float MouseSmoothing = 5.0f; // higher = smoother, slower response
    bool bInvertY = false;
    
    float SmoothedYaw = 0.f;
    float SmoothedPitch = 0.f;
    
    bool bIsToggling = false;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
    TSubclassOf<UAnimInstance> AnimBP;
    
    // Skeletal mesh component is already part of ACharacter (Mesh)
    
    // Animation asset to play (optional if using AnimBP)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimSequence* AnimationToPlay;
    
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
};
