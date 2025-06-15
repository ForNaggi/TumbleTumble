// Fill out your copyright notice in the Description page of Project Settings.


#include "ACheckPointBase.h"
#include "Components/BoxComponent.h"


// Sets default values
AACheckPointBase::AACheckPointBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName("Trigger");
	TriggerBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AACheckPointBase::BeginPlay()
{
	Super::BeginPlay();
	SetupTrigger();
}

void AACheckPointBase::SetupTrigger()
{
	//자식에서 오버라이드
}


