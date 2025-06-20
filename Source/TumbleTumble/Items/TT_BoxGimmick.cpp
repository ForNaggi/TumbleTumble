// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BoxGimmick.h"
#include "Components/BoxComponent.h"

ATT_BoxGimmick::ATT_BoxGimmick()
{
	bReplicates = true;

	// Root 설정
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
	BoxMesh->SetSimulatePhysics(true); // 박스는 물리 적용

	// OverlapPoint는 자식으로 부착
	OverlapPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapPoint"));
	OverlapPoint->SetupAttachment(BoxMesh);
	OverlapPoint->SetBoxExtent(FVector(20.f, 20.f, 20.f)); // 예시값, 블루프린트에서 조절 가능
	OverlapPoint->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapPoint->SetGenerateOverlapEvents(true);
}
