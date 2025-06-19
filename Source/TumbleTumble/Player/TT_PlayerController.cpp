// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"
#include "../Character/TT_CharacterPlayer.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//���� ���� �Ҷ� ���콺 �Ⱥ���
	FInputModeGameOnly GameModeOnly;
	SetInputMode(GameModeOnly);
}
/**
 * �� �����Ӹ��� ȣ��Ǹ� ���콺 �̵��� ���� ĳ���� �� ������ ������Ʈ�մϴ�.
 *
 * @param DeltaTime - ������ ���� �ð� ���� (��)
 *
 * �� �Լ��� ĳ���Ͱ� ���� ��� ���� �� ���콺�� ���� �̵���(MouseY)��
 * ĳ���Ϳ��� �����Ͽ� ���� ȸ�� ������ �����ϴ� �� ���˴ϴ�.
 */

void ATT_PlayerController::Tick(float Deltatime)
{
	float MouseX, MouseY;
	GetInputMouseDelta(MouseX, MouseY);

	if (ATT_CharacterPlayer* MyCharacter = Cast<ATT_CharacterPlayer>(GetPawn()))
	{
		//MyCharacter->UpdateArmDirection(MouseY); // �� ȸ�� ����
	}
}
