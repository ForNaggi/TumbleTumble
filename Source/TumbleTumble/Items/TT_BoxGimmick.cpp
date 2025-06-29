// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BoxGimmick.h"
#include "Components/BoxComponent.h"

ATT_BoxGimmick::ATT_BoxGimmick()
{
	bReplicates = true;
	SetReplicateMovement(true);
	// Root 설정
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
// 	BoxMesh->SetSimulatePhysics(true); // 박스는 물리 적용

}

void ATT_BoxGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BoxMesh->SetSimulatePhysics(true);
	}
}
