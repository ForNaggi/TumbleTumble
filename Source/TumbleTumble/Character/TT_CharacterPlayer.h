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


//애니메이션 
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
TSubclassOf<UAnimInstance> DefaultAnimInstance;

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


	//팔애니메이션 왼손
	void UpdateLeftLiftAnimation(bool bLifting);
	//팔애니메이션 오른쪽
	void UpdateRightLiftAnimation(bool bLifting);

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


//팔설정
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

	/** 물체 감지 거리 */
    UPROPERTY(EditDefaultsOnly)
    float GrabDistance = 300.f;
	/** 감지 채널 */
   
   UPROPERTY(EditDefaultsOnly)
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;



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



//잡기
public:
	/**
	 * ServerGrabObject는 서버에서 지정된 손(왼손 또는 오른손)으로 물체를 집도록 처리합니다.
	 *
	 * @param bIsLeft - true일 경우 왼손, false일 경우 오른손으로 판단합니다.
	 * @param TargetComp - 잡을 대상 컴포넌트 (UPrimitiveComponent 기반)
	 *
	 * 이 함수는 클라이언트에서 입력에 따라 호출되며, 서버에서 권한을 가지고 Grab 처리를 수행합니다.
	 * 예: 왼쪽 마우스를 눌렀을 때 물체를 서버 권한으로 Grab 처리할 때 사용됩니다.
	 */
UFUNCTION(Server, Reliable)
void ServerGrabObject(bool bIsLeft, UPrimitiveComponent* TargetComp);
/**
 * ServerReleaseObject는 서버에서 지정된 손(왼손 또는 오른손)의 물체 잡기를 해제하는 함수입니다.
 *
 * @param bIsLeft - true일 경우 왼손, false일 경우 오른손으로 판단합니다.
 *
 * 이 함수는 클라이언트가 마우스를 놓았을 때 서버에서 물체 잡기를 해제하기 위해 사용됩니다.
 * 예: 오른쪽 마우스 버튼에서 손을 뗐을 때 해당 손의 Grab을 해제할 때 사용됩니다.
 */
UFUNCTION(Server, Reliable)
void ServerReleaseObject(bool bIsLeft);
/**
 * MulticastGrabObject는 모든 클라이언트에서 지정된 손으로 물체를 Grab한 것처럼 보이게 동기화합니다.
 *
 * @param bIsLeft - true일 경우 왼손, false일 경우 오른손
 * @param TargetComp - Grab 대상 컴포넌트
 * @param GrabLoc - Grab 시 손이 향해야 할 위치 (손 소켓 기준 위치)
 *
 * 이 함수는 서버에서 GrabObject를 성공적으로 처리한 후, 클라이언트들에게 시각적 Grab을 동기화할 때 사용됩니다.
 * 예: 물체가 손 위치로 붙는 연출을 모두에게 보여줄 때 사용됩니다.
 */
UFUNCTION(NetMulticast, Unreliable)
void MulticastGrabObject(bool bIsLeft, UPrimitiveComponent* TargetComp, FVector GrabLoc);

/**
 * MulticastReleaseObject는 모든 클라이언트에서 Grab 해제를 동기화하는 함수입니다.
 *
 * @param bIsLeft - true일 경우 왼손, false일 경우 오른손
 *
 * 서버에서 Grab 해제가 일어난 후, 해당 손의 물체 잡기 상태를 모든 클라이언트에게 반영시켜주는 역할을 합니다.
 * 예: 물체가 손에서 떨어지는 연출을 모두가 보게 하고 싶을 때 사용됩니다.
 */
UFUNCTION(NetMulticast, Unreliable)
void MulticastReleaseObject(bool bIsLeft);

protected:

	/**
	 * TryGrab은 왼손 또는 오른손의 소켓 위치를 기준으로 앞 방향으로 트레이스를 쏴서
	 * 잡을 수 있는 물체가 있을 경우 PhysicsHandle을 통해 집는 함수입니다.
	 *
	 * @param bIsLeft - true일 경우 왼손, false일 경우 오른손 기준
	 */	
	 void TryGrabObject(bool bIsLeft);

	UPROPERTY(VisibleAnywhere, Category = "Grab")
TObjectPtr<class UPhysicsHandleComponent> LeftHandle;

UPROPERTY(VisibleAnywhere, Category = "Grab")
TObjectPtr<class UPhysicsHandleComponent> RightHandle;

};
