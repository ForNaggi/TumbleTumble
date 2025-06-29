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
 * OnRep_LeftMouseHeld는 bIsLeftMouseHeld가 서버에서 변경되어 클라이언트로 복제될 때 호출됩니다.
 *
 * @brief 왼손 들기 애니메이션 상태를 클라이언트에서 갱신합니다.
 */
UFUNCTION()
void OnRep_LeftMouseHeld();

/**
 * OnRep_RightMouseHeld는 bIsRightMouseHeld가 서버에서 변경되어 클라이언트로 복제될 때 호출됩니다.
 *
 * @brief 오른손 들기 애니메이션 상태를 클라이언트에서 갱신합니다.
 */
UFUNCTION()
void OnRep_RightMouseHeld();



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
	 * ServerGrabObject는 지정된 손의 PhysicsConstraintComponent를 사용해 타겟 물체와 물리적으로 연결하는 함수입니다.
	 *
	 * @param bIsLeft - true이면 왼손, false이면 오른손을 의미합니다.
	 * @param TargetComp - 손에 붙일 대상 UPrimitiveComponent (Physics Simulate가 활성화되어 있어야 함)
	 *
	 * 이 함수는 클라이언트 입력을 받은 후 서버에서 호출되며, 제약 연결 결과는 자동으로 클라이언트에 복제됩니다.
	 * 예: 캐릭터가 상자를 잡을 때 손에 달라붙는 효과를 구현할 수 있습니다.
	 */
	 UFUNCTION(Server, Reliable)
void ServerGrabObject(bool bIsLeft, UPrimitiveComponent* TargetComp);
/**
 * ServerReleaseObject는 지정된 손에 연결된 PhysicsConstraintComponent의 제약을 해제하는 함수입니다.
 *
 * @param bIsLeft - true이면 왼손, false이면 오른손을 의미합니다.
 *
 * 이 함수는 클라이언트 입력을 받은 후 서버에서 호출되며, 연결 해제 상태는 자동으로 클라이언트에 복제됩니다.
 * 예: 마우스 버튼을 놓았을 때 손이 물체에서 떨어지도록 할 수 있습니다.
 */
UFUNCTION(Server, Reliable)
void ServerReleaseObject(bool bIsLeft);

protected:

	/**
	 * TryGrabObject는 손 위치에서 전방으로 라인 트레이스를 실행하여 물체를 감지하고 서버에 집기 요청을 전송하는 함수입니다.
	 *
	 * @param bIsLeft - true이면 왼손, false이면 오른손을 의미합니다.
	 *
	 * 이 함수는 클라이언트에서 실행되며, 물체를 감지하면 ServerGrabObject를 호출하여 서버가 물리 제약을 연결하도록 합니다.
	 * 예: 마우스 클릭 시 가까운 상자를 손으로 잡게 하고 싶을 때 사용합니다.
	 */
	 // 	 void TryGrabObject(bool bIsLeft);

	 // 손 제약 컴포넌트
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab", meta = (AllowPrivateAccess = "true"))
TObjectPtr<class UPhysicsConstraintComponent> LeftConstraint;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab", meta = (AllowPrivateAccess = "true"))
TObjectPtr<class UPhysicsConstraintComponent> RightConstraint;


//잡은 물체를 저장하기 위한 컴포넌트 추가 
UPROPERTY(VisibleAnywhere)
TObjectPtr<class UTTGrabDetectorComponent> LeftGrabDetector;

UPROPERTY(VisibleAnywhere)
TObjectPtr<class UTTGrabDetectorComponent> RightGrabDetector;



};
