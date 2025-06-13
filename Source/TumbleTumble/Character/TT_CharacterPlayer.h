// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_CharacterBase.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "TT_CharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_CharacterPlayer : public ATT_CharacterBase
{
	GENERATED_BODY()

public:
	ATT_CharacterPlayer();

	virtual void BeginPlay() override;

	//인풋들 바인드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

//인풋
protected:
	

//카메라 
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

// input section
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Input , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;


};
