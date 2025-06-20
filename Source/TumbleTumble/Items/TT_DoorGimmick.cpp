// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_DoorGimmick.h"
#include "Components/StaticMeshComponent.h"

ATT_DoorGimmick::ATT_DoorGimmick()
{
	bReplicates = true;

	// 컴포넌트 구성
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMeshOpposite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshOpposite"));


	DoorMesh->SetupAttachment(RootComponent);
	DoorMeshOpposite->SetupAttachment(RootComponent);

	// 충돌
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DoorMeshOpposite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATT_DoorGimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorTimeline.TickTimeline(DeltaTime);
}

void ATT_DoorGimmick::OpenDoor()
{
	if (HasAuthority())
	{
		Multicast_OpenDoor();
	}
}

void ATT_DoorGimmick::CloseDoor()
{
	if (HasAuthority())
	{
		Multicast_CloseDoor();
	}


}

void ATT_DoorGimmick::BeginPlay()
{
	Super::BeginPlay();

	StartTransformLeft = DoorMesh->GetRelativeTransform();
	StartTransformRight = DoorMeshOpposite->GetRelativeTransform();

	if (MoveCurve)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("HandleTimelineProgress"));

		DoorTimeline.AddInterpFloat(MoveCurve, TimelineProgress);
		DoorTimeline.SetLooping(false);  // 단방향  
	}
}


void ATT_DoorGimmick::HandleTimelineProgress(float Value)
{
	const float OpenDistance = 50.f;

	FVector OffsetLeft = FVector(OpenDistance * Value, 0.f, 0.f);
	FVector OffsetRight = FVector(-OpenDistance * Value, 0.f, 0.f);

	DoorMesh->SetRelativeLocation(StartTransformLeft.GetLocation() + OffsetLeft);
	DoorMeshOpposite->SetRelativeLocation(StartTransformRight.GetLocation() + OffsetRight);
}
void ATT_DoorGimmick::Multicast_OpenDoor_Implementation()
{
	bIsOpening = true;
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMeshOpposite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	DoorTimeline.PlayFromStart();
}

void ATT_DoorGimmick::Multicast_CloseDoor_Implementation()
{
	bIsOpening = false;
	DoorTimeline.ReverseFromEnd();
}



