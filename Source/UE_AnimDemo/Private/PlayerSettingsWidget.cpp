//
//  PlayerSettingsWidget.cpp
//  
//
//  Created by Derrick Auyoung on 21/08/2025.
//
#include "PlayerSettingsWidget.h"
#include "Components/Border.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/BorderSlot.h"

// For advanced styling
#include "Engine/Texture2D.h"           // If using custom textures
#include "Materials/MaterialInterface.h"  // If using UI materials
#include "Styling/SlateBrush.h"         // For custom brush creation
#include "Styling/SlateColor.h"
#include "Engine/Font.h"

void UPlayerSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get the border widget (assuming it's bound)
    if (BackgroundBorder)
    {
        // Create a custom brush
        FSlateBrush CustomBrush;
        CustomBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
        CustomBrush.SetResourceObject(nullptr); // Use solid color
        CustomBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
        CustomBrush.OutlineSettings.CornerRadii = FVector4(10.0f, 10.0f, 10.0f, 10.0f); // Rounded corners
        
        // Set color with transparency
        FLinearColor BackgroundColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.85f);
        BackgroundColor.A = 0.8f; // 80% opacity
        BackgroundBorder->SetBrushColor(BackgroundColor);
        BackgroundBorder->SetPadding(FMargin(24.0f, 24.0f, 24.0f, 24.0f));
        
        // Apply consistent styling to all child widgets
        ApplyConsistentStyling();
    }
    
    if (SensitivitySlider)
    {
        SensitivitySlider->OnValueChanged.AddDynamic(this, &UPlayerSettingsWidget::HandleSensitivitySlider);
    }

    if (SmoothingSlider)
    {
        SmoothingSlider->OnValueChanged.AddDynamic(this, &UPlayerSettingsWidget::HandleSmoothingSlider);
    }

    if (InvertYCheckBox)
    {
        InvertYCheckBox->OnCheckStateChanged.AddDynamic(this, &UPlayerSettingsWidget::HandleInvertYChanged);
    }
    
    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UPlayerSettingsWidget::HandleCloseButtonPressed);
    }
}


void UPlayerSettingsWidget::HandleSensitivitySlider(float Value)
{
    OnSensitivityChanged.Broadcast(Value);
}


void UPlayerSettingsWidget::HandleSmoothingSlider(float Value)
{
    OnSmoothingChanged.Broadcast(Value);
}


void UPlayerSettingsWidget::HandleInvertYChanged(bool bChecked)
{
    OnInvertYChanged.Broadcast(bChecked);
}


void UPlayerSettingsWidget::HandleCloseButtonPressed()
{
    OnCloseButtonPressed.Broadcast();
}


void UPlayerSettingsWidget::SetMouseSensitivity(float NewSens)
{
    // Update UI slider, etc.
    if (SensitivitySlider)
        SensitivitySlider->SetValue(NewSens);
}


void UPlayerSettingsWidget::SetMouseSmoothing(float NewSmooth)
{
    if (SmoothingSlider)
        SmoothingSlider->SetValue(NewSmooth);
}


void UPlayerSettingsWidget::SetInvertY(bool bInvert)
{
    if (InvertYCheckBox)
        InvertYCheckBox->SetIsChecked(bInvert);
}


void UPlayerSettingsWidget::ApplyConsistentStyling()
{
    // Define your consistent styling values
    const FMargin StandardPadding(8.0f, 4.0f, 8.0f, 4.0f);
    const FLinearColor TextColor = FLinearColor::White;
    const float StandardFontSize = 14.0f;
    
    // Style all child widgets recursively
    StyleChildWidgets(BackgroundBorder, StandardPadding, TextColor, StandardFontSize);
}

void UPlayerSettingsWidget::StyleChildWidgets(UWidget* ParentWidget, const FMargin& Padding, const FLinearColor& TextColor, float FontSize)
{
    if (!ParentWidget) return;
    
    // Check if this widget is a panel that contains other widgets
    if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(ParentWidget))
    {
        // Iterate through all child widgets
        for (int32 i = 0; i < PanelWidget->GetChildrenCount(); ++i)
        {
            UWidget* ChildWidget = PanelWidget->GetChildAt(i);
            if (!ChildWidget) continue;
            
            // Apply padding based on slot type (different slots have different methods)
            if (UPanelSlot* Slot = ChildWidget->Slot)
            {
                // Try different slot types that support padding
                if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(Slot))
                {
                    VBoxSlot->SetPadding(Padding);
                }
                else if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(Slot))
                {
                    HBoxSlot->SetPadding(Padding);
                }
                else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Slot))
                {
                    BorderSlot->SetPadding(Padding);
                }
            }
            
            // Style specific widget types using modern API
            if (UTextBlock* TextBlock = Cast<UTextBlock>(ChildWidget))
            {
                // Style text blocks
                TextBlock->SetColorAndOpacity(TextColor);
                
                // Set font size
                FSlateFontInfo FontInfo = TextBlock->GetFont();
                FontInfo.Size = FontSize;
                TextBlock->SetFont(FontInfo);
            }
            else if (UButton* Button = Cast<UButton>(ChildWidget))
            {
                // Style buttons - if GetStyle() doesn't work, use basic properties
                // Note: Advanced button styling is often better done in UMG Designer
                
                // Try to set background color (this may or may not work depending on UE version)
                Button->SetBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
                
                // Style button text if it has any
                if (Button->GetChildrenCount() > 0)
                {
                    if (UTextBlock* ButtonText = Cast<UTextBlock>(Button->GetChildAt(0)))
                    {
                        ButtonText->SetColorAndOpacity(TextColor);
                        FSlateFontInfo FontInfo = ButtonText->GetFont();
                        FontInfo.Size = FontSize;
                        ButtonText->SetFont(FontInfo);
                    }
                }
            }
            else if (USlider* Slider = Cast<USlider>(ChildWidget))
            {
                // Style sliders - USlider doesn't have GetStyle(), use direct property access
                Slider->SetSliderBarColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f));
                Slider->SetSliderHandleColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
                
                // Optional: Set other slider properties
                Slider->SetStepSize(0.01f); // Smooth sliding
            }
            else if (UCheckBox* CheckBox = Cast<UCheckBox>(ChildWidget))
            {
                // Style checkboxes - UCheckBox also uses direct property setters
                CheckBox->SetCheckedState(CheckBox->GetCheckedState()); // Refresh current state
                
                // Note: UCheckBox styling is limited in C++, most styling should be done in UMG Designer
                // You can set basic properties but complex styling requires Blueprint or UMG Designer
            }
            
            // Recursively style children of this widget
            StyleChildWidgets(ChildWidget, Padding, TextColor, FontSize);
        }
    }
}
