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
    // �θ� Ŭ������ �⺻ ó�� ������ �����ϱ� ���� Super�� ���� ȣ���մϴ�.
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    if (NewPlayer == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NewPlayer is null in HandleStartingNewPlayer."));
        return;
    }

    /**
     * ChoosePlayerStart �Լ��� �÷��̾ ������ PlayerStart ���͸� �����մϴ�.
     * ���������δ� �� ���/���� ���� ������ ����� �� �ֽ��ϴ�.
     * ���⼭ ���� ��ġ�� �������� �÷��̾ �ϴ� ���� ��ġ�մϴ�.
     */
    AActor* StartSpot = ChoosePlayerStart(NewPlayer);

    if (!StartSpot)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid PlayerStart found!"));
        return;
    }

    /**
     * ���� ��ġ ���:
     * PlayerStart�� ��ġ���� InitialSpawnHeight ��ŭ ���� �����
     * �ϴÿ��� �������� �ڿ������� ���۵ǵ��� �մϴ�.
     */
    FVector SpawnLocation = StartSpot->GetActorLocation() + FVector(0.f, 0.f, InitialSpawnHeight);
    FRotator SpawnRotation = StartSpot->GetActorRotation();

    /**
     * RestartPlayerAtTransform �Լ��� ������ ��ġ(Transform)�� �÷��̾ �����մϴ�.
     * PlayerController�� ������� Pawn�� �����ϰ� ��ġ�� �����մϴ�.
     */
    RestartPlayerAtTransform(NewPlayer, FTransform(SpawnRotation, SpawnLocation));

}
