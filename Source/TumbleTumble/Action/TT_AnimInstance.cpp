// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_AnimInstance.h"

void UTT_AnimInstance::SetLeftLifting(bool bNewState)
{
	bLiftLeft = bNewState;
}

void UTT_AnimInstance::SetRightLifting(bool bNewState)
{
	bLiftRight = bNewState;
}

void UTT_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}
