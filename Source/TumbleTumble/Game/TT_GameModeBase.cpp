// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_GameModeBase.h"

ATT_GameModeBase::ATT_GameModeBase()
{

	static ConstructorHelpers::FClassFinder<APawn> DefaultCharacterRef(
		TEXT("/Game/TumbleTown/BluePrint/Player/BP_PLayer.BP_PLayer_C"));

	if (DefaultCharacterRef.Class)
	{
		DefaultPawnClass = DefaultCharacterRef.Class;
	}


	// PlayerControllerClass.

	static ConstructorHelpers::FClassFinder<APlayerController>PlayerControllerRef(TEXT("/Script/TumbleTumble.TT_PlayerController"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}

void ATT_GameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    // 부모 클래스의 기본 처리 로직을 유지하기 위해 Super를 먼저 호출합니다.
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    if (NewPlayer == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NewPlayer is null in HandleStartingNewPlayer."));
        return;
    }

    /**
     * ChoosePlayerStart 함수는 플레이어가 스폰할 PlayerStart 액터를 선택합니다.
     * 내부적으로는 팀 기반/랜덤 스폰 전략을 사용할 수 있습니다.
     * 여기서 얻은 위치를 기준으로 플레이어를 하늘 위에 배치합니다.
     */
    AActor* StartSpot = ChoosePlayerStart(NewPlayer);

    if (!StartSpot)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid PlayerStart found!"));
        return;
    }

    /**
     * 스폰 위치 계산:
     * PlayerStart의 위치에서 InitialSpawnHeight 만큼 위로 띄워서
     * 하늘에서 떨어지듯 자연스럽게 시작되도록 합니다.
     */
    FVector SpawnLocation = StartSpot->GetActorLocation() + FVector(0.f, 0.f, InitialSpawnHeight);
    FRotator SpawnRotation = StartSpot->GetActorRotation();

    /**
     * RestartPlayerAtTransform 함수는 지정한 위치(Transform)에 플레이어를 스폰합니다.
     * PlayerController를 기반으로 Pawn을 생성하고 위치를 설정합니다.
     */
    RestartPlayerAtTransform(NewPlayer, FTransform(SpawnRotation, SpawnLocation));

}
