// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGrabDetectorComponent.h"

UTTGrabDetectorComponent::UTTGrabDetectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetActive(true);
}



void UTTGrabDetectorComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("GrabDetector BeginPlay!"));

    OnComponentBeginOverlap.AddDynamic(this, &UTTGrabDetectorComponent::HandleBeginOverlap);
    OnComponentEndOverlap.AddDynamic(this, &UTTGrabDetectorComponent::HandleEndOverlap);
}

UPrimitiveComponent* UTTGrabDetectorComponent::GetCurrentTarget() const
{
    return CurrentTarget;
}

void UTTGrabDetectorComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsActive() || !OtherComp) return;
    if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        CurrentTarget = OtherComp;
        UE_LOG(LogTemp, Warning, TEXT("GrabDetector 오버랩 감지: %s"), *OtherComp->GetName());
    }
}

void UTTGrabDetectorComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherComp && OtherComp == CurrentTarget)
    {
        CurrentTarget = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("GrabDetector 오버랩 해제: %s"), *OtherComp->GetName());
    }
}


