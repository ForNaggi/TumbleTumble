// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BoxGimmick.h"
#include "Components/BoxComponent.h"

ATT_BoxGimmick::ATT_BoxGimmick()
{
	bReplicates = true;

	// Root ����
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
	BoxMesh->SetSimulatePhysics(true); // �ڽ��� ���� ����

	// OverlapPoint�� �ڽ����� ����
	OverlapPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapPoint"));
	OverlapPoint->SetupAttachment(BoxMesh);
	OverlapPoint->SetBoxExtent(FVector(20.f, 20.f, 20.f)); // ���ð�, �������Ʈ���� ���� ����
	OverlapPoint->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapPoint->SetGenerateOverlapEvents(true);
}
