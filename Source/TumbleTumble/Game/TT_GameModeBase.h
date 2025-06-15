// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TT_GameModeBase.generated.h"

/**
 *
 */
UCLASS()
class TUMBLETUMBLE_API ATT_GameModeBase : public AGameMode
{
	GENERATED_BODY()

public:

	ATT_GameModeBase();

	/**
		* HandleStartingNewPlayer�� ���ο� �÷��̾ ������ �������� �� ȣ��Ǵ� �Լ��Դϴ�.
		*
		* @param NewPlayer ������ �÷��̾��� APlayerController �������Դϴ�.
		*        �� ��Ʈ�ѷ��� ���� �÷��̾��� Pawn�� �����ϰ� �ʱ�ȭ�� �� �ֽ��ϴ�.
		*
		* �� �Լ��� PlayerStart ��ġ�� ������� ������ ���̸�ŭ ���� ����� �÷��̾ �����մϴ�.
		* ���� ���� �� �ϴÿ��� �������� ������ �����ϰ� ���� �� �����ϰ� ����� �� �ֽ��ϴ�.
		*/
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:

	/**
	   * �÷��̾��� �ʱ� ���� �����Դϴ�.
	   * PlayerStart ���� ���� ����(Z��)���� �󸶳� ������� �����մϴ�.
	   * �⺻���� 2000.f�̸�, �����Ϳ��� ���� �����մϴ�.
	   */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float InitialSpawnHeight = 2000.f;

};
