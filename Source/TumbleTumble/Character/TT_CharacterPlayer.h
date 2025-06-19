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

	virtual void PossessedBy(AController* NewController) override;


	//인풋들 바인드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);




protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	//손설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LeftHandAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RightHandAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ArmVerticalAction;

	//일어나기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> StandUpAction;


	//레그돌 컴포넌트 
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ragdoll, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTT_RagdollComponent> RagdollComponent;


	UFUNCTION()
	void OnStandUpInput();


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

	/** 서버에서 클라이언트로 복제되는 마지막 저장 위치 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FVector LastSavePoint;

protected:
	/** 낙사 후 리스폰 대기 상태를 추적하기 위한 플래그 */
	bool bIsFallingToDeath = false;

	/** 리스폰 타이머 핸들 */
	FTimerHandle RespawnTimerHandle;

	//피지컬 애니메이션 컴포넌트 
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPhysicalAnimationComponent> PhysicalAnimation;


	//팔설정 
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTT_ArmTargetComponent> LeftArmTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTT_ArmTargetComponent> RightArmTarget;

	/**
	 * 왼손 수직 입력값을 받아 제어하는 함수입니다.
	 * 마우스를 위아래로 움직여 팔을 위로 들거나 내릴 수 있습니다.
	 * @param Value - EnhancedInput에서 전달된 1D 또는 2D 입력값
	 */
	UFUNCTION()
	void ControlLeftArmVertical(const FInputActionValue& Value);

	/**
	 * 왼쪽 마우스 버튼을 눌렀을 때 호출됩니다.
	 * 이 상태에서는 왼팔 조작이 가능해집니다.
	 */
	UFUNCTION()
	void OnLeftMousePressed(const FInputActionValue& Value);

	/**
	 * 왼쪽 마우스 버튼에서 손을 뗐을 때 호출됩니다.
	 */
	UFUNCTION()
	void OnLeftMouseReleased(const FInputActionValue& Value);

	/**
 * 오른쪽 마우스 버튼을 눌렀을 때 호출됩니다.
 * 이 상태에서는 왼팔 조작이 가능해집니다.
 */
	UFUNCTION()
	void OnRightMousePressed(const FInputActionValue& Value);

	/**
	 * 오른쪽 마우스 버튼에서 손을 뗐을 때 호출됩니다.
	 */
	UFUNCTION()
	void OnRightMouseReleased(const FInputActionValue& Value);



protected:
	/**
	 * 왼쪽 마우스가 눌린 상태를 추적하는 변수입니다. (서버에서도 동기화됨)
	 */
	UPROPERTY(Replicated)
	bool bIsLeftMouseHeld = false;
	/**
 * 오른쪽 마우스가 눌린 상태를 추적하는 변수입니다. (서버에서도 동기화됨)
 */
	UPROPERTY(Replicated)
	bool bIsRightMouseHeld = false;

	/**
 * Server_SetLeftMouseHeld는 서버에게 bIsLeftMouseHeld 상태를 변경하도록 요청합니다 (RPC).
 *
 * @param bNewState - true: 누름 상태, false: 해제 상태
 *
 * 이 함수는 클라이언트에서 호출 시 서버에서 bIsLeftMouseHeld를 변경합니다.
 */
	UFUNCTION(Server, Reliable)
	void Server_SetLeftMouseHeld(bool bNewState);

	//위 와 동일 하게 오른쪽 손 

	UFUNCTION(Server, Reliable)
	void Server_SetRightMouseHeld(bool bNewState);

};
