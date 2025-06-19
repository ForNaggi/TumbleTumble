// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_ArmTargetComponent.h"


// Sets default values for this component's properties
UTT_ArmTargetComponent::UTT_ArmTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UTT_ArmTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTT_ArmTargetComponent::AdjustFromInput(float Vertical, float Horizontal)
{
	FVector Relative = GetRelativeLocation();
	Relative += FVector(Vertical * 5.f, Horizontal * 5.f, 0.f); // 마우스 값에 따라 이동
	SetRelativeLocation(Relative);
}


