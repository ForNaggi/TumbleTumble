// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//���� ���� �Ҷ� ���콺 �Ⱥ���
	FInputModeGameOnly GameModeOnly;
	SetInputMode(GameModeOnly);
}
