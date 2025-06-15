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
	/** 모든 Prop이 사용할 트리거 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	class UBoxComponent* TriggerBox;

	/** BeginPlay 진입 시 공통 처리 */
	virtual void BeginPlay() override;
	/** 트리거 설정을 자식 클래스가 오버라이드해서 구현 */
	virtual void SetupTrigger();


};
