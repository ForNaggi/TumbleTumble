// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TT_ArmTargetComponent.generated.h"

/**
 * 양팔 타겟 위치를 담당하는 컴포넌트입니다.
 * 주로 IK 또는 PhysicsConstraint의 타겟으로 사용됩니다.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TUMBLETUMBLE_API UTT_ArmTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTT_ArmTargetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	 * AdjustFromInput는 마우스 입력값에 따라 팔 타겟 컴포넌트의 상대 위치를 조절하는 함수입니다.
	 *
	 * @param Vertical - 상하 마우스 입력값 (Y축 기준 조정)
	 * @param Horizontal - 좌우 마우스 입력값 (X축 기준 조정)
	 *
	 * 이 함수는 플레이어가 마우스 움직임을 통해 팔을 뻗는 방향을 제어하고 싶을 때 사용될 수 있습니다.
	 * 예: Human: Fall Flat 스타일의 팔 뻗기 제어를 구현할 때 등
	 */
	void AdjustFromInput(float Vertical, float Horizontal);


};
