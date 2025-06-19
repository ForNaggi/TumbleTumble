// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_GimIckBase.h"


// Sets default values
ATT_GimIckBase::ATT_GimIckBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates =true;
}

// Called when the game starts or when spawned
void ATT_GimIckBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_GimIckBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ATT_GimIckBase::HasAuthorityOnServer() const
{
	return HasAuthority(); // Server 권한 판단
}

