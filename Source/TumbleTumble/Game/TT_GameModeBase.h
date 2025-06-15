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
		* HandleStartingNewPlayer는 새로운 플레이어가 서버에 접속했을 때 호출되는 함수입니다.
		*
		* @param NewPlayer 접속한 플레이어의 APlayerController 포인터입니다.
		*        이 컨트롤러를 통해 플레이어의 Pawn을 스폰하고 초기화할 수 있습니다.
		*
		* 이 함수는 PlayerStart 위치를 기반으로 지정한 높이만큼 위로 띄워서 플레이어를 스폰합니다.
		* 게임 시작 시 하늘에서 떨어지는 연출을 구현하고 싶을 때 유용하게 사용할 수 있습니다.
		*/
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:

	/**
	   * 플레이어의 초기 스폰 높이입니다.
	   * PlayerStart 기준 위쪽 방향(Z축)으로 얼마나 띄울지를 지정합니다.
	   * 기본값은 2000.f이며, 에디터에서 수정 가능합니다.
	   */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float InitialSpawnHeight = 2000.f;

};
