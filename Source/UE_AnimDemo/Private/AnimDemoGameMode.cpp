#include "AnimDemoGameMode.h"
#include "AnimTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAnimDemoGameMode::AAnimDemoGameMode()
{
    // Replace with your BP path
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/AnimCppCharacter"));
    if (PlayerPawnBPClass.Succeeded())
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
