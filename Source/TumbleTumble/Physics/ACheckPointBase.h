// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACheckPointBase.generated.h"

UCLASS()
class TUMBLETUMBLE_API AACheckPointBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AACheckPointBase();

protected:
	/** ��� Prop�� ����� Ʈ���� �ڽ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	class UBoxComponent* TriggerBox;

	/** BeginPlay ���� �� ���� ó�� */
	virtual void BeginPlay() override;
	/** Ʈ���� ������ �ڽ� Ŭ������ �������̵��ؼ� ���� */
	virtual void SetupTrigger();


};
