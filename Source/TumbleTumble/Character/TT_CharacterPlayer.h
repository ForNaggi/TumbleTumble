// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_CharacterBase.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "TT_CharacterPlayer.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckpointUpdated, FVector, NewSavePoint);

/**
 * 세이브 포인트가 갱신될 때 발생하는 델리게이트입니다.
 * CheckpointZone 등 외부 Prop에서 바인딩하여 호출합니다.
 */
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

	virtual void Tick(float Deltatime) override;

	//인풋들 바인드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	//인풋
protected:


	//카메라 
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

	// input section
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	//세이브 포인트
public:

    /**
     * 세이브 위치를 업데이트합니다.
     * @param NewSaveLocation - 새로운 체크포인트 위치 (보통 플레이어 상공으로 오프셋 포함)
     * 이 함수는 델리게이트를 통해 호출되며, 세이브 위치를 저장하는 데 사용됩니다.
     */
    UFUNCTION()
    void UpdateCheckpoint(FVector NewSaveLocation);

    /**
     * 낙사 이후 일정 시간 뒤 세이브 위치로 리스폰합니다.
     * 이 함수는 서버에서만 호출되며, 플레이어 컨트롤러를 통해 위치를 갱신합니다.
     */
    UFUNCTION()
    void RespawnAtCheckpoint();



	/**
		 * OnCheckpointUpdated는 외부에서 체크포인트 위치를 알려줄 때 사용하는 델리게이트입니다.
		 * ex) ACheckpointZone이 플레이어에게 새로운 세이브 위치를 알릴 때 사용됩니다.
		 */
	UPROPERTY(BlueprintAssignable, Category = "Save")
	FOnCheckpointUpdated OnCheckpointUpdated;

	/** 마지막으로 저장된 세이브 위치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FVector LastSavePoint;

protected:
	/** 낙사 후 리스폰 대기 상태를 추적하기 위한 플래그 */
	bool bIsFallingToDeath = false;

	/** 리스폰 타이머 핸들 */
	FTimerHandle RespawnTimerHandle;

};
