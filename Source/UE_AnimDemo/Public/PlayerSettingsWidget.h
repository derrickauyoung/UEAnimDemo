#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Components/Border.h"  // Add this
#include "Layout/Margin.h"      // Add this for FMargin
#include "Math/Color.h"         // Add this for FLinearColor
#include "PlayerSettingsWidget.generated.h"

// Delegate types
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatChanged, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoolChanged, bool, Value);

/**
 * Widget for player settings
 */
UCLASS()
class UE_ANIMDEMO_API UPlayerSettingsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** Delegates to call when sliders/checkbox change */
    UPROPERTY(BlueprintAssignable, Category="Settings")
    FOnFloatChanged OnSensitivityChanged;

    UPROPERTY(BlueprintAssignable, Category="Settings")
    FOnFloatChanged OnSmoothingChanged;

    UPROPERTY(BlueprintAssignable, Category="Settings")
    FOnBoolChanged OnInvertYChanged;
    
    UPROPERTY(BlueprintAssignable, Category="Settings")
    FOnButtonClickedEvent OnCloseButtonPressed;
    
    UFUNCTION(BlueprintCallable)
    void SetMouseSensitivity(float NewSens);

    UFUNCTION(BlueprintCallable)
    void SetMouseSmoothing(float NewSmooth);

    UFUNCTION(BlueprintCallable)
    void SetInvertY(bool bInvert);

protected:
    /** Bind your sliders and checkbox here */
    UPROPERTY(meta=(BindWidget))
    USlider* SensitivitySlider;

    UPROPERTY(meta=(BindWidget))
    USlider* SmoothingSlider;

    UPROPERTY(meta=(BindWidget))
    UCheckBox* InvertYCheckBox;
    
    UPROPERTY(meta=(BindWidget))
    UButton* CloseButton;
    
    UPROPERTY(meta=(BindWidget))
    UBorder* BackgroundBorder;

private:
    /** Internal handlers for broadcasting delegates */
    UFUNCTION()
    void HandleSensitivitySlider(float Value);

    UFUNCTION()
    void HandleSmoothingSlider(float Value);

    UFUNCTION()
    void HandleInvertYChanged(bool bChecked);
    
    UFUNCTION()
    void HandleCloseButtonPressed();
    
    UPROPERTY()
    UPlayerSettingsWidget* SettingsWidget;
    
    /** Styling functions */
    void ApplyConsistentStyling();
    void StyleChildWidgets(UWidget* ParentWidget, const FMargin& Padding, const FLinearColor& TextColor, float FontSize);  // for button click
};
