// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_RagdollComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"




// Sets default values for this component's properties
UTT_RagdollComponent::UTT_RagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    HeightForRagdoll = 600.f;
}


// Called when the game starts
void UTT_RagdollComponent::BeginPlay()
{
	Super::BeginPlay();

    if (!SKMesh)
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            SKMesh = OwnerCharacter->GetMesh();
            CapsuleComponent = OwnerCharacter->GetCapsuleComponent();
            CharacterMovement = OwnerCharacter->GetCharacterMovement();
        }
    }
}

void UTT_RagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
   Super::TickComponent(DeltaTime,TickType, ThisTickFunction);

   DeltaSeconds = DeltaTime;

   // 1. 서버가 아닐 경우 early return (클라에서 물리 trace 없음)
   if (!GetOwner()->HasAuthority())
   {
       return;
   }
   if (SKMesh && CapsuleComponent)
   {
       SKMesh->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
   }
   // 2. 레그돌 상태일 때 캡슐 따라가기만 수행
//    if (bRagdoll)
//    {
//        if (bCanMove && CapsuleComponent && SKMesh)
//        {
//            const FVector SocketLocation = SKMesh->GetSocketLocation(SocketName);
//            CapsuleComponent->SetWorldLocation(SocketLocation, false, nullptr, ETeleportType::TeleportPhysics);
//        }
//        return;
//    }

   // 3. 레그돌 전 상태에서 추락 감지
   if (!bShouldCheckFallToRagdoll || !SKMesh)
   {
       return;
   }

   // 4. 일정 높이 아래에 지면이 없을 경우 레그돌 전환
   const FVector Start = SKMesh->GetComponentLocation();
   const FVector End = Start - FVector(0.f, 0.f, HeightForRagdoll);

   FHitResult HitResult;
   FCollisionQueryParams Params;
   Params.AddIgnoredActor(GetOwner());

   const bool bHit = GetWorld()->LineTraceSingleByChannel(
       HitResult,
       Start,
       End,
       ECC_Visibility,
       Params
   );

   if (!bHit)
   {
       Server_ActivateRagdoll();

       // 다시 체크하지 않도록 끔 (일어서면 다시 true로 켜짐)
       bShouldCheckFallToRagdoll = false;
   }
}



void UTT_RagdollComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UTT_RagdollComponent, bRagdoll);
    DOREPLIFETIME(UTT_RagdollComponent, bCanMove);
}

void UTT_RagdollComponent::StandUp()
{
    if (GetOwner()->HasAuthority())
    {
        Multicast_StandUp();
    }
    else
    {
        Server_StandUp();
    }
}

void UTT_RagdollComponent::StandUp_Internal()
{
    if (!SKMesh || !CapsuleComponent) return;

    // 1. 물리 해제 (중요!)
    SKMesh->SetSimulatePhysics(false);

    // 1. 캐릭터가 앞으로 누웠는지 뒤로 누웠는지 판단
    const FVector Start = SKMesh->GetSocketLocation(FName("Spine2")); // 적절한 본 위치
    const FVector End = Start + SKMesh->GetForwardVector() * 50.0f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    const bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(5.0f),
        Params
    );

    // 2. 누운 방향에 따라 애님 몽타주 선택
    UAnimMontage* MontageToPlay = bHit ? GettingUpAnimationFront : GettingUpAnimationBack;

    // 3. 몽타주 재생
    if (MontageToPlay && SKMesh->GetAnimInstance())
    {
        SKMesh->GetAnimInstance()->Montage_Play(MontageToPlay);
    }

    // 4. 레그돌 상태 해제
    bRagdoll = false;
    // 메시 분리
    SKMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    // 방향 판별 후 필요한 경우 Yaw 회전 보정
    if (bHit == false) // 예: 등으로 누운 상태면 앞으로 돌리기
    {
        SKMesh->AddLocalRotation(FRotator(0.f, 180.f, 0.f));
    }

    // 메시 재부착 및 원래 상대 위치로 복원
    SKMesh->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
   
   SKMesh->SetRelativeTransform(RelativeTransform);
//     // 8. 상대 위치/회전 재설정 (정상 상태 기준)
    SKMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//     SKMesh->SetRelativeScale3D(RelativeTransform.GetScale3D());
    // 9. 로그
    UE_LOG(LogTemp, Warning, TEXT(">>> Capsule Rotation: %s"), *CapsuleComponent->GetComponentRotation().ToString());
    UE_LOG(LogTemp, Warning, TEXT(">>> Mesh World Rotation: %s"), *SKMesh->GetComponentRotation().ToString());
    UE_LOG(LogTemp, Warning, TEXT(">>> Mesh Forward Vector: %s"), *SKMesh->GetForwardVector().ToString());
    // 7. 정상 이동 가능하도록 설정
    bCanMove = true;

    // 8. 필요하다면 이후 상태 초기화 함수 호출
    NormalWalkingRagdoll(); // 이동, 충돌 설정 복구 등
}

void UTT_RagdollComponent::Multicast_StandUp_Implementation()
{
    if (!CharacterMovement || !SKMesh || !CapsuleComponent) return;
    if (CharacterMovement->IsFalling()) return;
    if (!bRagdoll) return;

    if (UAnimInstance* AnimInstance = SKMesh->GetAnimInstance())
    {
        AnimInstance->SavePoseSnapshot(TEXT("Ragdoll"));
    }

    StandUp_Internal();
}

void UTT_RagdollComponent::Server_StandUp_Implementation()
{
    Multicast_StandUp();
}


void UTT_RagdollComponent::Server_SetRagdoll_Implementation(USkeletalMeshComponent* InMesh, UCapsuleComponent* InCapsule, const FName& InSocketName, UPhysicalAnimationComponent* InPhysicalAnim, const FName& InRightFoot, const FName& InLeftFoot, UCharacterMovementComponent* InMovement, bool bInUseActiveRagdoll)
{
    
    if (!GetOwner()->HasAuthority()) return;

    SKMesh = InMesh;
    CapsuleComponent = InCapsule;
    SocketName = InSocketName;
    PhysicalAnimationComponent = InPhysicalAnim;
    RightFoot = InRightFoot;
    LeftFoot = InLeftFoot;
    CharacterMovement = InMovement;
    bUseActiveRagdollWhenWalking = bInUseActiveRagdoll;

    // 상대 트랜스폼 저장 (레그돌이 아직 아니어야 정상 회전 저장됨)
    if (!bRagdoll && IsValid(SKMesh))
    {
        RelativeTransform = SKMesh->GetRelativeTransform();

        UE_LOG(LogTemp, Warning, TEXT(">> [Setup] RelativeTransform 저장 시점"));
        UE_LOG(LogTemp, Warning, TEXT(">> Location: %s"), *RelativeTransform.GetLocation().ToString());
        UE_LOG(LogTemp, Warning, TEXT(">> Rotation: %s"), *RelativeTransform.GetRotation().Rotator().ToString());
    }

    // 초기 세팅 (예: 물리 애니메이션 초기화 등)
    if (PhysicalAnimationComponent && SKMesh)
    {
        PhysicalAnimationComponent->SetSkeletalMeshComponent(SKMesh);
    }
    // 기본 ragdoll 상태
    NormalWalkingRagdoll();
}

void UTT_RagdollComponent::Server_ActivateRagdoll_Implementation()
{
    if (bRagdoll || !IsValid(SKMesh.Get()) || !IsValid(PhysicalAnimationComponent.Get()))
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("RagDoll상태임"));

    // 1. 물리 애니메이션 설정 적용

    FPhysicalAnimationData PhysicalAnimData;
    PhysicalAnimData.bIsLocalSimulation = true;
    PhysicalAnimData.OrientationStrength = 0.0f;
    PhysicalAnimData.AngularVelocityStrength = 300.0f;
    PhysicalAnimData.PositionStrength = 300.0f;
    PhysicalAnimData.VelocityStrength = 300.0f;
    PhysicalAnimData.MaxLinearForce = 0.0f;
    PhysicalAnimData.MaxAngularForce = 0.0f;

    PhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(
        SocketName, PhysicalAnimData, true
    );

    // 2. 시뮬레이션 시작(물리적용)
    //SKMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    SKMesh->SetAllBodiesSimulatePhysics(true);
    SKMesh->SetCollisionProfileName(TEXT("Pawn"));
    SKMesh->SetAllBodiesBelowSimulatePhysics(SocketName, true, true);
//     SKMesh->SetAnimationMode(EAnimationMode::AnimationCustomMode);
    //CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캡슐 충돌 완전 끔


//     // 3. 이동 비활성화
//     if (CharacterMovement)
//     {
//         CharacterMovement->DisableMovement();
//         CharacterMovement->StopMovementImmediately();
//     }
    // 3. 상태 변수 설정 (레플리케이션 변수)
    bRagdoll = true;
    bCanMove = false;
}

void UTT_RagdollComponent::NormalWalkingRagdoll()
{
    if (!SKMesh || !PhysicalAnimationComponent) {
        UE_LOG(LogTemp, Error, TEXT("SKMesh or PhysicalAnimationComponent is NULL!"));
    return;
    }

    // 1. 활성 상태 여부 확인
    if (bUseActiveRagdollWhenWalking)
    {
        UE_LOG(LogTemp, Warning, TEXT("NormalWalkingRagdoll() start"));


        // 2. 물리 애니메이션 설정 구성
        FPhysicalAnimationData AnimData;
        AnimData.bIsLocalSimulation = true;
        AnimData.OrientationStrength = 1000.f;
        AnimData.AngularVelocityStrength = 100.f;
        AnimData.PositionStrength = 1000.f;
        AnimData.VelocityStrength = 100.f;
        AnimData.MaxLinearForce = 0.f;
        AnimData.MaxAngularForce = 0.f;

        // 3. 설정 적용
        PhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(SocketName, AnimData, true);

        // 4. 레그돌 적용
        SKMesh->SetAllBodiesBelowSimulatePhysics(SocketName, true, false);

        SKMesh->SetAllBodiesBelowSimulatePhysics(*LeftFoot.ToString(), false, true);
        SKMesh->SetAllBodiesBelowSimulatePhysics(*RightFoot.ToString(), false, true);
        UE_LOG(LogTemp, Warning, TEXT("NormalWalkingRagdoll() 적용 완료"));
    }
    else
    {
        // 레그돌을 비활성화하고자 할 경우 설정 해제 (선택 사항)
        //SKMesh->SetAllBodiesBelowSimulatePhysics(*SocketName.ToString(), false, true);
        UE_LOG(LogTemp, Warning, TEXT("bUseActiveRagdollWhenWalking = false. 흐물거림 비활성화됨"));
    }
}

