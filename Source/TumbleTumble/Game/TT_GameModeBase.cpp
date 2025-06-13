// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_GameModeBase.h"

ATT_GameModeBase::ATT_GameModeBase()
{

	static ConstructorHelpers::FClassFinder<APawn> DefaultCharacterRef(
		TEXT("/Game/TumbleTumble/BluePrint/Player/BP_PLayer.BP_PLayer_C"));

	if (DefaultCharacterRef.Class)
	{
		DefaultPawnClass = DefaultCharacterRef.Class;
	}


	// PlayerControllerClass.

	static ConstructorHelpers::FClassFinder<APlayerController>PlayerControllerRef(TEXT("/Script/TumbleTown.TT_PlayerController"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}
