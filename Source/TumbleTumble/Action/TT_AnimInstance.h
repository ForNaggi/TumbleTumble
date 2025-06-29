// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TT_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API UTT_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
    UFUNCTION(BlueprintCallable)
    void SetLeftLifting(bool bNewState);

    UFUNCTION(BlueprintCallable)
    void SetRightLifting(bool bNewState);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lift")
    bool bLiftLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lift")
    bool bLiftRight;
	
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
