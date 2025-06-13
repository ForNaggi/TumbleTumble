// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TT_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_PlayerController : public APlayerController
{
	GENERATED_BODY()

public: 

	virtual void BeginPlay() override;
	
};
