// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TT_GimmickInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTT_GimmickInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TUMBLETUMBLE_API ITT_GimmickInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
     * 캐릭터가 기믹과 상호작용할 때 호출됩니다.
     *
     * @param Interactor - 상호작용한 캐릭터
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gimmick")
    void Interact(AActor* Interactor);
	
};
