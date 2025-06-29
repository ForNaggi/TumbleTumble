// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "TTGrabDetectorComponent.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API UTTGrabDetectorComponent : public USphereComponent
{
	GENERATED_BODY()
public:
    UTTGrabDetectorComponent();


    UPrimitiveComponent* GetCurrentTarget()const;

protected:
    virtual void BeginPlay() override;

    /** 오버랩 시작 처리 */
    UFUNCTION()
    void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /** 오버랩 종료 시 */
    UFUNCTION()
    void HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
	    /** 현재 오버랩 중인 대상 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UPrimitiveComponent> CurrentTarget= nullptr;

	
};
