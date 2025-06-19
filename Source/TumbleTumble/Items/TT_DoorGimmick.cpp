// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_DoorGimmick.h"
#include "Components/StaticMeshComponent.h"

ATT_DoorGimmick::ATT_DoorGimmick()
{
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = DoorMesh;
}

void ATT_DoorGimmick::Multicast_OpenDoor_Implementation()
{
    // 예시: 문 회전 열기
    FRotator NewRotation = FRotator(0.f, 90.f, 0.f); // 열리는 방향
    DoorMesh->SetRelativeRotation(NewRotation);
}

void ATT_DoorGimmick::OpenDoor()
{
    if (HasAuthority())
    {
        Multicast_OpenDoor();
    }
}
