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
    // TickComponent ����ȭ�� �߶� ���� ���� 
    bool bShouldCheckFallToRagdoll = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** ���̷�Ż �޽� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class USkeletalMeshComponent> SKMesh;

    /** ĸ�� �ݸ��� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class  UCapsuleComponent> CapsuleComponent;

    /** ������ �ִϸ��̼� ������Ʈ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UPhysicalAnimationComponent> PhysicalAnimationComponent;

    /** ĳ���� �����Ʈ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UCharacterMovementComponent> CharacterMovement;

    /** ��� ��ġ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FTransform RelativeTransform;

    /** ���� �̸� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName SocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName RightFoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    FName LeftFoot;

    /** Ÿ�� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class AActor> Target;

    /** ���׵� ���� ���� �Ǵܿ� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    float HeightForRagdoll;

    /** ��Ÿ Ÿ�� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    float DeltaSeconds;

    /** �տ��� �Ͼ�� �ִ� ��Ÿ�� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UAnimMontage> GettingUpAnimationFront;

    /** �ڿ��� �Ͼ�� �ִ� ��Ÿ�� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    TObjectPtr <class UAnimMontage> GettingUpAnimationBack;

    /** ������ �� �ִ°� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    bool bCanMove;

    /** �ȴ� �� ��Ƽ�� ���׵� ��� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    bool bUseActiveRagdollWhenWalking;

    /** �ǰ� ��� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ragdoll")
    AActor* HitActor;

      UPROPERTY(BlueprintReadWrite, Replicated)
    bool bRagdoll = false;




    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
 * Server_SetRagdoll�� ���׵� �ý��ۿ� �ʿ��� ��� ������Ʈ �� �������� �������� �ʱ�ȭ�ϴ� �Լ��Դϴ�.
 *
 * @param InMesh - ����� SkeletalMeshComponent (ĳ���� ��ü)
 * @param InCapsule - ����� CapsuleComponent (�ݸ��� ������Ʈ)
 * @param InSocketName - ���� ��� ��ġ�� ����� ���� �̸� (ex. "pelvis")
 * @param InPhysicalAnim - ���� �ִϸ��̼��� ������ PhysicalAnimationComponent
 * @param InRightFoot - ������ �� ���� �̸� (ex. "foot_r")
 * @param InLeftFoot - ���� �� ���� �̸� (ex. "foot_l")
 * @param InMovement - ĳ���� �̵��� ����� CharacterMovementComponent
 * @param bInUseActiveRagdoll - �ȴ� ���� �ɵ��� ���׵� ��� ����
 *
 * �� �Լ��� Ŭ���̾�Ʈ���� ������ ��û�Ǿ� ����Ǹ�,
 * ���������� ������ �����Ƿ� Dedicated Server ȯ�濡�� �����ϰ� ���� �� �ֽ��ϴ�.
 * ĳ���Ͱ� ������ �� �ʿ��� ������Ʈ���� �������� ���ε����� �� ����մϴ�.
 * ��: ĳ���� BeginPlay ���� �������� ���׵� �ý����� �ʱ�ȭ�ϰ� ���� ��
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
 * StandUp�� ĳ���Ͱ� ���� ����ִ��� Ȯ���ϰ�, ���߿� �� ������ ����ĳ��Ʈ �� ���� ���� �̻��� ��� ���׵� ���¸� Ȱ��ȭ�մϴ�.
 *
 * @return ����
 *
 * �� �Լ��� ƽ���� �ֱ������� ȣ��Ǹ�, ���������� ����Ǿ� ĳ���Ͱ� ���� ������ �� �ڵ����� ���׵� ���·� ���Խ�Ű�� �� �����մϴ�.
 * ��: �������� �ٴڿ� ���� �ʾ��� �� ���� �Ÿ� �̻��̸� Ragdoll�� ��ȯ.
 */    
UFUNCTION(BlueprintCallable)
void StandUp();
UFUNCTION()
void StandUp_Internal();
//�⺻ ���׵�  ���� �Լ� 
UFUNCTION(BlueprintCallable)
void NormalWalkingRagdoll();


UFUNCTION(Server, Reliable)
void Server_StandUp();

UFUNCTION(NetMulticast, Reliable)
void Multicast_StandUp();
  /** ActivateRagdoll�� ĳ���͸� ���׵� ���·� ��ȯ�մϴ�.
     *
     * @return ����
     *
     * �� �Լ��� �������� ȣ��Ǿ�� �ϸ�, ���ø����̼��� ���� Ŭ���̾�Ʈ�� ����˴ϴ�.
     */
    UFUNCTION(Server, Reliable)
    void Server_ActivateRagdoll();

	
};
