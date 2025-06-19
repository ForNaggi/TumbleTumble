// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"
#include "../Character/TT_CharacterPlayer.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//게임 시작 할때 마우스 안보임
	FInputModeGameOnly GameModeOnly;
	SetInputMode(GameModeOnly);
}
/**
 * 매 프레임마다 호출되며 마우스 이동에 따라 캐릭터 팔 방향을 업데이트합니다.
 *
 * @param DeltaTime - 프레임 간의 시간 간격 (초)
 *
 * 이 함수는 캐릭터가 팔을 드는 중일 때 마우스의 수직 이동량(MouseY)을
 * 캐릭터에게 전달하여 팔의 회전 방향을 조정하는 데 사용됩니다.
 */

void ATT_PlayerController::Tick(float Deltatime)
{
	float MouseX, MouseY;
	GetInputMouseDelta(MouseX, MouseY);

	if (ATT_CharacterPlayer* MyCharacter = Cast<ATT_CharacterPlayer>(GetPawn()))
	{
		//MyCharacter->UpdateArmDirection(MouseY); // 팔 회전 각도
	}
}
