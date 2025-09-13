//
//  AnimCppChar.cpp
//  
//
//  Created by Derrick Auyoung on 23/08/2025.
//
#include "AnimCppChar.h"
#include "MyAnimInstance.h"
#include "AnimationStateMachine.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSettingsSave.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

AAnimCppChar::AAnimCppChar()
{
    PrimaryActorTick.bCanEverTick = true;
    
    CurrentAnimState = ECharacterAnimState::Idle;
    CurrentBlendSpaceInput = 0.f;
    
    // Set the custom AnimInstance class
    GetMesh()->SetAnimInstanceClass(UMyAnimInstance::StaticClass());
    
    // Create animation state machine
    //AnimStateMachine = CreateDefaultSubobject<UAnimationStateMachine>(TEXT("AnimStateMachine"));
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Characters/SKM_Manny.SKM_Manny"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
        GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // Align with capsule
        GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load skeletal mesh! Check the path."));
    }
    
    // Create spring arm
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // how far back the camera follows
    CameraBoom->bUsePawnControlRotation = true; // rotate the arm based on controller

    // Create follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // camera doesn't rotate relative to arm
}

void AAnimCppChar::BeginPlay()
{
    Super::BeginPlay();
    
    // Cache AnimInstance
    OwningAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (!OwningAnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimInstance is not of type UMyAnimInstance!"));
    }
    
    // Debug: Check if animations are loaded
    UE_LOG(LogTemp, Warning, TEXT("IdleAnimation: %s"), IdleAnimation ? *IdleAnimation->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("WalkAnimation: %s"), WalkAnimation ? *WalkAnimation->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("RunAnimation: %s"), RunAnimation ? *RunAnimation->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("JumpAnimation: %s"), JumpAnimation ? *JumpAnimation->GetName() : TEXT("NULL"));
    
    SetupAnimationStateMachine();
    
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("Successfully added mapping context"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is null!"));
            }
        }
        
        if (SettingsWidgetClass) // UPROPERTY TSubclassOf<UPlayerSettingsWidget> set in editor
        {
            SettingsWidget = CreateWidget<UPlayerSettingsWidget>(GetWorld(), SettingsWidgetClass);
            if (SettingsWidget)
            {
                UE_LOG(LogTemp, Warning, TEXT("SettingsWidget created successfully."));
                SettingsWidget->AddToViewport();
                SettingsWidget->SetVisibility(ESlateVisibility::Hidden);

                // Bind delegates to character functions
                SettingsWidget->OnSensitivityChanged.AddDynamic(this, &AAnimCppChar::SetMouseSensitivity);
                SettingsWidget->OnSmoothingChanged.AddDynamic(this, &AAnimCppChar::SetMouseSmoothing);
                SettingsWidget->OnInvertYChanged.AddDynamic(this, &AAnimCppChar::SetInvertY);
                SettingsWidget->OnCloseButtonPressed.AddDynamic(this, &AAnimCppChar::ToggleSettingsMenu);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("SettingsWidget creation FAILED!"));
            }
        }
        
        // Load Player Settings
        LoadPlayerSettings();
    }
}

void AAnimCppChar::SetupAnimationStateMachine()
{
    if (!AnimStateMachine)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimStateMachine is null!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Setting up animation state machine"));
    
    // Initialize the state machine
    AnimStateMachine->Initialize(GetMesh());
    
    // Register animations for each state
    if (IdleAnimation)
    {
        AnimStateMachine->RegisterStateAnimation(ECharacterAnimState::Idle, IdleAnimation, true, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("Registered Idle animation"));
    }
    /*
    if (WalkAnimation)
    {
        AnimStateMachine->RegisterStateAnimation(ECharacterAnimState::Walk, WalkAnimation, true, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("Registered Walk animation"));
    }
    
    if (RunAnimation)
    {
        AnimStateMachine->RegisterStateAnimation(ECharacterAnimState::Run, RunAnimation, true, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("Registered Run animation"));
    } */
    
    if (JumpAnimation)
    {
        AnimStateMachine->RegisterStateAnimation(ECharacterAnimState::Jump, JumpAnimation, false, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("Registered Jump animation"));
    }
    
    // Instead of separate walk/run animations, use a blend space
    if (MovementBlendSpace)
    {
        AnimStateMachine->RegisterStateBlendSpace(
            ECharacterAnimState::Locomotion,
            MovementBlendSpace,
            true,
            1.0f
        );
        UE_LOG(LogTemp, Warning, TEXT("Registered BlendSpace animation"));
    }
    
    // Setup transitions with lambda conditions
    AnimStateMachine->AddTransition(
        ECharacterAnimState::Idle,
        ECharacterAnimState::Locomotion,
        [this]() { return ShouldWalk(); }
    );
    
   
    AnimStateMachine->AddTransition(
        ECharacterAnimState::Locomotion,
        ECharacterAnimState::Idle,
        [this]() { return ShouldIdle(); }
    );
    
   
    // Jump transitions from any ground state
    AnimStateMachine->AddTransition(
        ECharacterAnimState::Idle,
        ECharacterAnimState::Jump,
        [this]() { return ShouldJump(); }
    );
    
    AnimStateMachine->AddTransition(
        ECharacterAnimState::Locomotion,
        ECharacterAnimState::Jump,
        [this]() { return ShouldJump(); }
    );

}

void AAnimCppChar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update animation inputs
    UpdateAnimationInputs();
    UpdateAnimationState(DeltaTime);
    
    // Tick the state machine
    if (AnimStateMachine)
    {
        AnimStateMachine->Tick(DeltaTime);
    }
}

void AAnimCppChar::UpdateAnimationInputs()
{
    /*
    if (AnimStateMachine)
    {
        // Set blend space inputs based on movement speed
        float Speed = GetVelocity().Size();
        AnimStateMachine->SetBlendSpaceInput(Speed);
    }
     */
    
    if (!OwningAnimInstance) return;

    FString DisplayName = UEnum::GetDisplayValueAsText(CurrentAnimState).ToString();
    UE_LOG(LogTemp, Warning, TEXT("Current Anim State = %s"), *DisplayName);
    
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;

    float Speed = GetVelocity().Size();
    bool bIsInAir = MoveComp->IsFalling();
    bool bIsMoving = Speed > 10.f;

    CurrentBlendSpaceInput = Speed;

    OwningAnimInstance->SetLocomotionBlendSpaceInput(Speed);
    OwningAnimInstance->SetJumping(bIsInAir);
    OwningAnimInstance->SetIdle(!bIsMoving && !bIsInAir);
}

void AAnimCppChar::UpdateAnimationState(float DeltaTime)
{
    if (!OwningAnimInstance) return;

    bool bIsInAir = GetCharacterMovement()->IsFalling();

    if (bIsInAir)
    {
        CurrentAnimState = ECharacterAnimState::Jump;
    }
    else if (CurrentBlendSpaceInput > 10.f)
    {
        CurrentAnimState = ECharacterAnimState::Locomotion;
    }
    else
    {
        CurrentAnimState = ECharacterAnimState::Idle;
    }

    OwningAnimInstance->SetCurrentAnimState(CurrentAnimState);
}

// Transition condition implementations
bool AAnimCppChar::ShouldWalk() const
{
    float Speed = GetVelocity().Size();
    return Speed > 10.0f && Speed < 300.0f && GetCharacterMovement()->IsMovingOnGround();
}

bool AAnimCppChar::ShouldRun() const
{
    float Speed = GetVelocity().Size();
    return Speed >= 300.0f && GetCharacterMovement()->IsMovingOnGround();
}

bool AAnimCppChar::ShouldJump() const
{
    return GetCharacterMovement()->IsFalling() && GetVelocity().Z > 0.0f;
}

bool AAnimCppChar::ShouldIdle() const
{
    float Speed = GetVelocity().Size();
    return Speed <= 10.0f && GetCharacterMovement()->IsMovingOnGround();
}

void AAnimCppChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // First, let's verify we're getting the Enhanced Input Component
    //UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent called"));

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Successfully cast to EnhancedInputComponent"));
        
        if (IA_Jump)
        {
            //UE_LOG(LogTemp, Warning, TEXT("IA_Jump is valid, attempting to bind..."));
            EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
            //UE_LOG(LogTemp, Warning, TEXT("BindAction completed"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_Jump is NULL - IA_Jump Input Action not assigned!"));
        }
        if (IA_Move)
        {
            //UE_LOG(LogTemp, Warning, TEXT("IA_Move is valid, attempting to bind..."));
            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAnimCppChar::Move);
            //UE_LOG(LogTemp, Warning, TEXT("BindAction completed"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_Move is NULL - IA_Move Input Action not assigned!"));
        }
        
        // Rotation
        if (IA_Turn)
        {
            EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AAnimCppChar::Turn);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_Turn is NULL - IA_Turn Input Action not assigned!"))
        }

        if (IA_LookUp)
        {
            EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &AAnimCppChar::LookUp);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_LookUp is NULL - IA_LookUp Input Action not assigned!"))
        }
        
        if (IA_ToggleSettings)
        {
            EnhancedInputComponent->BindAction(IA_ToggleSettings, ETriggerEvent::Started, this, &AAnimCppChar::ToggleSettingsMenu);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_ToggleSettings is NULL - IA_ToggleSettings Input Action not assigned!"))
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to EnhancedInputComponent"));
    }
}


void AAnimCppChar::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    /*
     UE_LOG(LogTemp, Warning, TEXT("Controller=%s (Type: %s) MovementVector=(%.2f, %.2f)"),
        Controller ? *Controller->GetName() : TEXT("nullptr"),
        Controller ? *Controller->GetClass()->GetName() : TEXT("N/A"),
        MovementVector.X, MovementVector.Y);
    */
    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //UE_LOG(LogTemp, Warning, TEXT("Move X=%.2f Y=%.2f"), MovementVector.X, MovementVector.Y);
        AddMovementInput(ForwardDir, MovementVector.X);
        AddMovementInput(RightDir, MovementVector.Y);
    }
}


void AAnimCppChar::Turn(const FInputActionValue& Value)
{
    const float RawYaw = Value.Get<float>() * MouseSensitivity;
    SmoothedYaw = FMath::FInterpTo(SmoothedYaw, RawYaw, GetWorld()->GetDeltaSeconds(), MouseSmoothing);
    AddControllerYawInput(SmoothedYaw);
}

void AAnimCppChar::LookUp(const FInputActionValue& Value)
{
    float RawPitch = Value.Get<float>() * MouseSensitivity;

    if (bInvertY)
    {
        RawPitch *= -1.0f;
    }

    SmoothedPitch = FMath::FInterpTo(SmoothedPitch, RawPitch, GetWorld()->GetDeltaSeconds(), MouseSmoothing);

    AddControllerPitchInput(SmoothedPitch);
}


void AAnimCppChar::SavePlayerSettings()
{
    UPlayerSettingsSave* SaveGameInstance = Cast<UPlayerSettingsSave>(
        UGameplayStatics::CreateSaveGameObject(UPlayerSettingsSave::StaticClass())
    );

    SaveGameInstance->MouseSensitivity = MouseSensitivity;
    SaveGameInstance->MouseSmoothing = MouseSmoothing;
    SaveGameInstance->bInvertY = bInvertY;

    UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSettings"), 0);
}


void AAnimCppChar::LoadPlayerSettings()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSettings"), 0))
    {
        UPlayerSettingsSave* Loaded = Cast<UPlayerSettingsSave>(
            UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0)
        );

        if (Loaded)
        {
            MouseSensitivity = Loaded->MouseSensitivity;
            MouseSmoothing = Loaded->MouseSmoothing;
            bInvertY = Loaded->bInvertY;
        }
    }
}


void AAnimCppChar::HideSettingsWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Saving Settings and Hiding Settings Window..."));
    SavePlayerSettings();
    
    SettingsWidget->SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("Settings saved."));
}


void AAnimCppChar::ShowSettingsWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Showing Settings Window and Loading Settings."));
    
    SettingsWidget->SetVisibility(ESlateVisibility::Visible);
    LoadPlayerSettings();
    
    // Update UI with loaded values
    SettingsWidget->SetMouseSensitivity(MouseSensitivity);
    SettingsWidget->SetMouseSmoothing(MouseSmoothing);
    SettingsWidget->SetInvertY(bInvertY);
    
    UE_LOG(LogTemp, Warning, TEXT("Settings loaded."));
}

void AAnimCppChar::ToggleSettingsMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleSettingsMenu called!"));
    // Add small debounce to skip rapid key repeats
    if (bIsToggling) return;

    bIsToggling = true;
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        bIsToggling = false;
    });
    
    if (!SettingsWidget) return;
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    const bool bIsVisible = SettingsWidget->IsVisible();
    
    UE_LOG(LogTemp, Warning, TEXT("bIsVisible=%s"), bIsVisible ? TEXT("true") : TEXT("false"));
    if (bIsVisible)
    {
        HideSettingsWidget();
        
        GetWorld()->GetTimerManager().SetTimerForNextTick([this, PC]()
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            PC->SetIgnoreLookInput(false);      // Ensure look input is restored
            PC->SetIgnoreMoveInput(false);      // Ensure move input is restored
        });
    }
    else
    {
        ShowSettingsWidget();

        // Set UI input mode with slight delay
        GetWorld()->GetTimerManager().SetTimerForNextTick([this, PC]()
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(SettingsWidget->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        });
    }
}


void AAnimCppChar::SetMouseSensitivity(float Value)
{
    MouseSensitivity = FMath::Clamp(Value, 0.1f, 10.0f);
}


void AAnimCppChar::SetMouseSmoothing(float Value)
{
    MouseSmoothing = FMath::Clamp(Value, 0.f, 20.f);
}


void AAnimCppChar::SetInvertY(bool bInvert)
{
    bInvertY = bInvert;
}
