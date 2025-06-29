// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_GimIckBase.h"
#include "TT_BoxGimmick.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_BoxGimmick : public ATT_GimIckBase
{
	GENERATED_BODY()
	
	
public:
	ATT_BoxGimmick();

	void virtual BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> BoxMesh;

};
