// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TT_ArmTargetComponent.generated.h"

/**
 * ���� Ÿ�� ��ġ�� ����ϴ� ������Ʈ�Դϴ�.
 * �ַ� IK �Ǵ� PhysicsConstraint�� Ÿ������ ���˴ϴ�.
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
	 * AdjustFromInput�� ���콺 �Է°��� ���� �� Ÿ�� ������Ʈ�� ��� ��ġ�� �����ϴ� �Լ��Դϴ�.
	 *
	 * @param Vertical - ���� ���콺 �Է°� (Y�� ���� ����)
	 * @param Horizontal - �¿� ���콺 �Է°� (X�� ���� ����)
	 *
	 * �� �Լ��� �÷��̾ ���콺 �������� ���� ���� ���� ������ �����ϰ� ���� �� ���� �� �ֽ��ϴ�.
	 * ��: Human: Fall Flat ��Ÿ���� �� ���� ��� ������ �� ��
	 */
	void AdjustFromInput(float Vertical, float Horizontal);


};
