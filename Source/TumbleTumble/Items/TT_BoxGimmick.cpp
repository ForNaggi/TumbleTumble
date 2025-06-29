// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BoxGimmick.h"
#include "Components/BoxComponent.h"

ATT_BoxGimmick::ATT_BoxGimmick()
{
	bReplicates = true;
	SetReplicateMovement(true);
	// Root ����
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
// 	BoxMesh->SetSimulatePhysics(true); // �ڽ��� ���� ����

}

void ATT_BoxGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BoxMesh->SetSimulatePhysics(true);
	}
}
