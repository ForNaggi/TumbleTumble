// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_GimIckBase.h"
#include "TT_DoorGimmick.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_DoorGimmick : public ATT_GimIckBase
{
	GENERATED_BODY()
	
public:
    ATT_DoorGimmick();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OpenDoor();

    UFUNCTION(BlueprintCallable)
    void OpenDoor();

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	
};
