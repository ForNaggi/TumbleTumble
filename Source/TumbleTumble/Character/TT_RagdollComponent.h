// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT_RagdollComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TUMBLETUMBLE_API UTT_RagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTT_RagdollComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    // TickComponent 최적화용 추락 감지 변수 
    bool bShouldCheckFallToRagdoll = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 스켈레탈 메시 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class USkeletalMeshComponent> SKMesh;

    /** 캡슐 콜리전 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class  UCapsuleComponent> CapsuleComponent;

    /** 피지컬 애니메이션 컴포넌트 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UPhysicalAnimationComponent> PhysicalAnimationComponent;

    /** 캐릭터 무브먼트 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UCharacterMovementComponent> CharacterMovement;

    /** 상대 위치 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FTransform RelativeTransform;

    /** 소켓 이름 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName SocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName RightFoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName LeftFoot;

    /** 타겟 액터 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class AActor> Target;

    /** 래그돌 상태 높이 판단용 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    float HeightForRagdoll;

    /** 델타 타임 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    float DeltaSeconds;

    /** 앞에서 일어나는 애님 몽타주 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UAnimMontage> GettingUpAnimationFront;

    /** 뒤에서 일어나는 애님 몽타주 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UAnimMontage> GettingUpAnimationBack;

    /** 움직일 수 있는가 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    bool bCanMove;

    /** 걷는 중 액티브 래그돌 사용 여부 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    bool bUseActiveRagdollWhenWalking;

    /** 피격 대상 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    AActor* HitActor;

      UPROPERTY(BlueprintReadWrite, Replicated)
    bool bRagdoll = false;




    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
 * Server_SetRagdoll는 래그돌 시스템에 필요한 모든 컴포넌트 및 설정값을 서버에서 초기화하는 함수입니다.
 *
 * @param InMesh - 사용할 SkeletalMeshComponent (캐릭터 본체)
 * @param InCapsule - 사용할 CapsuleComponent (콜리전 컴포넌트)
 * @param InSocketName - 땅에 닿는 위치로 사용할 소켓 이름 (ex. "pelvis")
 * @param InPhysicalAnim - 물리 애니메이션을 제어할 PhysicalAnimationComponent
 * @param InRightFoot - 오른쪽 발 소켓 이름 (ex. "foot_r")
 * @param InLeftFoot - 왼쪽 발 소켓 이름 (ex. "foot_l")
 * @param InMovement - 캐릭터 이동에 사용할 CharacterMovementComponent
 * @param bInUseActiveRagdoll - 걷는 동안 능동형 래그돌 사용 여부
 *
 * 이 함수는 클라이언트에서 서버로 요청되어 실행되며,
 * 서버에서만 권한을 가지므로 Dedicated Server 환경에서 안전하게 사용될 수 있습니다.
 * 캐릭터가 스폰된 후 필요한 컴포넌트들을 서버에서 바인딩해줄 때 사용합니다.
 * 예: 캐릭터 BeginPlay 이후 서버에서 래그돌 시스템을 초기화하고 싶을 때
 */

    UFUNCTION(BlueprintCallable, Server, Reliable)
void Server_SetRagdoll(
    USkeletalMeshComponent* InMesh,
    UCapsuleComponent* InCapsule,
    const FName& InSocketName,
    UPhysicalAnimationComponent* InPhysicalAnim,
    const FName& InRightFoot,
    const FName& InLeftFoot,
    UCharacterMovementComponent* InMovement,
    bool bInUseActiveRagdoll
);

/**
 * StandUp은 캐릭터가 땅에 닿아있는지 확인하고, 공중에 떠 있으면 레이캐스트 후 일정 높이 이상일 경우 래그돌 상태를 활성화합니다.
 *
 * @return 없음
 *
 * 이 함수는 틱에서 주기적으로 호출되며, 서버에서만 실행되어 캐릭터가 낙하 도중일 때 자동으로 래그돌 상태로 진입시키는 데 유용합니다.
 * 예: 떨어져서 바닥에 닿지 않았을 때 일정 거리 이상이면 Ragdoll로 전환.
 */    
UFUNCTION(BlueprintCallable)
void StandUp();
UFUNCTION()
void StandUp_Internal();
//기본 래그돌  상태 함수 
UFUNCTION(BlueprintCallable)
void NormalWalkingRagdoll();


UFUNCTION(Server, Reliable)
void Server_StandUp();

UFUNCTION(NetMulticast, Reliable)
void Multicast_StandUp();
  /** ActivateRagdoll는 캐릭터를 래그돌 상태로 전환합니다.
     *
     * @return 없음
     *
     * 이 함수는 서버에서 호출되어야 하며, 레플리케이션을 통해 클라이언트에 적용됩니다.
     */
    UFUNCTION(Server, Reliable)
    void Server_ActivateRagdoll();

	
};
