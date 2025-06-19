// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_ButtonGimmick.h"
#include "Net/UnrealNetwork.h"

ATT_ButtonGimmick::ATT_ButtonGimmick()
{
}

void ATT_ButtonGimmick::Interact_Implementation(AActor* Interactor)
{
}

void ATT_ButtonGimmick::Server_PressButton_Implementation(AActor* Interactor)
{
}
void ATT_ButtonGimmick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATT_ButtonGimmick, LinkedDoors);
}