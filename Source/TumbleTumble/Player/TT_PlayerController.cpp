// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//게임 시작 할때 마우스 안보임
	FInputModeGameOnly GameModeOnly;
	SetInputMode(GameModeOnly);
}
