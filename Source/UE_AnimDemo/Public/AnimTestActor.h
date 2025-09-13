#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimSequence.h" // Needed for UAnimSequence
#include "AnimTestActor.generated.h"

UCLASS()
class UE_ANIMDEMO_API AAnimTestActor : public AActor
{
    GENERATED_BODY()

public:
    AAnimTestActor();
    
protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* SkeletalMeshComp;
    
    // The animation sequence we want to play (editable in editor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimSequence* AnimationToPlay;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimationAsset* RunAnim;
};
