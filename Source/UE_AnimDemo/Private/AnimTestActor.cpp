#include "AnimTestActor.h"
#include "Components/SkeletalMeshComponent.h"

AAnimTestActor::AAnimTestActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create the skeletal mesh component and make it the root
    SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
    RootComponent = SkeletalMeshComp;
}

void AAnimTestActor::BeginPlay()
{
    Super::BeginPlay();

    if (AnimationToPlay)
    {
        SkeletalMeshComp->PlayAnimation(AnimationToPlay, true); // true = loop
    }
    
    if (RunAnim)
    {
        SkeletalMeshComp->PlayAnimation(RunAnim, true);
    }
}

void AAnimTestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Move forward constantly
    AddActorWorldOffset(GetActorForwardVector() * 100.f * DeltaTime);
}
