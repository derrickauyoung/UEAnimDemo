// AnimTestCharacter.cpp
#include "AnimTestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSettingsSave.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"


AAnimTestCharacter::AAnimTestCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Character already has Mesh (USkeletalMeshComponent*) and CapsuleComponent
    // Optional: set mesh
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

    // Optional: set default movement speed
    GetCharacterMovement()->MaxWalkSpeed = 200.f;

    if (AnimBP)
    {
        GetMesh()->SetAnimInstanceClass(AnimBP);
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


void AAnimTestCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
                //UE_LOG(LogTemp, Warning, TEXT("Successfully added mapping context"));
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
                SettingsWidget->OnSensitivityChanged.AddDynamic(this, &AAnimTestCharacter::SetMouseSensitivity);
                SettingsWidget->OnSmoothingChanged.AddDynamic(this, &AAnimTestCharacter::SetMouseSmoothing);
                SettingsWidget->OnInvertYChanged.AddDynamic(this, &AAnimTestCharacter::SetInvertY);
                SettingsWidget->OnCloseButtonPressed.AddDynamic(this, &AAnimTestCharacter::ToggleSettingsMenu);
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


void AAnimTestCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AAnimTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAnimTestCharacter::Move);
            //UE_LOG(LogTemp, Warning, TEXT("BindAction completed"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_Move is NULL - IA_Move Input Action not assigned!"));
        }
        
        // Rotation
        if (IA_Turn)
        {
            EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AAnimTestCharacter::Turn);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_Turn is NULL - IA_Turn Input Action not assigned!"))
        }

        if (IA_LookUp)
        {
            EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &AAnimTestCharacter::LookUp);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("IA_LookUp is NULL - IA_LookUp Input Action not assigned!"))
        }
        
        if (IA_ToggleSettings)
        {
            EnhancedInputComponent->BindAction(IA_ToggleSettings, ETriggerEvent::Started, this, &AAnimTestCharacter::ToggleSettingsMenu);
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


void AAnimTestCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    /* UE_LOG(LogTemp, Warning, TEXT("Controller=%s (Type: %s) MovementVector=(%.2f, %.2f)"),
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

        // UE_LOG(LogTemp, Warning, TEXT("Move X=%.2f Y=%.2f"), MovementVector.X, MovementVector.Y);
        AddMovementInput(ForwardDir, MovementVector.X);
        AddMovementInput(RightDir, MovementVector.Y);
    }
}


void AAnimTestCharacter::Turn(const FInputActionValue& Value)
{
    float TargetYaw = Value.Get<float>() * MouseSensitivity;
    SmoothedYaw = FMath::FInterpTo(SmoothedYaw, TargetYaw, GetWorld()->GetDeltaSeconds(), MouseSmoothing);
    AddControllerYawInput(SmoothedYaw);
}

void AAnimTestCharacter::LookUp(const FInputActionValue& Value)
{
    float RawPitch = Value.Get<float>() * MouseSensitivity;

    // Apply invert Y if enabled
    if (bInvertY)
    {
        RawPitch *= -1.0f;
    }

    SmoothedPitch = FMath::FInterpTo(SmoothedPitch, RawPitch, GetWorld()->GetDeltaSeconds(), MouseSmoothing);
    AddControllerPitchInput(SmoothedPitch);
}


void AAnimTestCharacter::SavePlayerSettings()
{
    UPlayerSettingsSave* SaveGameInstance = Cast<UPlayerSettingsSave>(
        UGameplayStatics::CreateSaveGameObject(UPlayerSettingsSave::StaticClass())
    );

    SaveGameInstance->MouseSensitivity = MouseSensitivity;
    SaveGameInstance->MouseSmoothing = MouseSmoothing;
    SaveGameInstance->bInvertY = bInvertY;

    UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSettings"), 0);
}


void AAnimTestCharacter::LoadPlayerSettings()
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


void AAnimTestCharacter::HideSettingsWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Saving Settings and Hiding Settings Window..."));
    SavePlayerSettings();
    
    SettingsWidget->SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("Settings saved."));
}


void AAnimTestCharacter::ShowSettingsWidget()
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

void AAnimTestCharacter::ToggleSettingsMenu()
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


void AAnimTestCharacter::SetMouseSensitivity(float Value)
{
    MouseSensitivity = FMath::Clamp(Value, 0.1f, 10.0f);
}


void AAnimTestCharacter::SetMouseSmoothing(float Value)
{
    MouseSmoothing = FMath::Clamp(Value, 0.f, 20.f);
}


void AAnimTestCharacter::SetInvertY(bool bInvert)
{
    bInvertY = bInvert;
}
