// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_GimIckBase.h"
#include "../Interface/TT_GimmickInteractable.h"
#include "TT_DoorGimmick.h"
#include "TT_ButtonGimmick.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_ButtonGimmick : public ATT_GimIckBase, public ITT_GimmickInteractable
{
	GENERATED_BODY()
	
public:
    ATT_ButtonGimmick();

    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(Server, Reliable)
    void Server_PressButton(AActor* Interactor);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
    UPROPERTY(EditAnywhere, Replicated)
    TArray<TObjectPtr<ATT_DoorGimmick>> LinkedDoors;
	
};
