// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GimIckBase.generated.h"

UCLASS()
class TUMBLETUMBLE_API ATT_GimIckBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATT_GimIckBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** �� ����� �������� �۵� �������� Ȯ���� �� ����� �� �ִ� ���� */
    UFUNCTION(BlueprintCallable, Category = "Gimmick")
    bool HasAuthorityOnServer() const;	
	
};
