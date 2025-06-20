// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_CharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


#include "GameFrameWork/CharacterMovementComponent.h"
#include "../Game/TT_GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "TT_ArmTargetComponent.h"
#include "TT_RagdollComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "../Action/TT_AnimInstance.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

ATT_CharacterPlayer::ATT_CharacterPlayer()
{
	// 컨트롤러의 회전을 받아서 설정하는 모드를 모두 해제.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 무브먼트 설정.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	// 컴포넌트 설정.
	//GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -88.f),
		FRotator(0.0f, -90.0f, 0.0f)
	);
	GetMesh()->SetRelativeScale3D(FVector(0.0525f));
	// 리소스 설정.
// 	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/TumbleTown/Character/NewFolder2/HumanFallFlat.HumanFallFlat"));
// 	if (CharacterMesh.Object)
// 	{
// 		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
// 	}

	// Animation Blueprint 설정.
// 	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(TEXT("/Game/TumbleTown/Animation/Animation2/AB_RagdollMesh_Skeleton.AB_RagdollMesh_Skeleton_C"));
// 	if (CharacterAnim.Class)
// 	{
// 		GetMesh()->SetAnimClass(CharacterAnim.Class);
// 	}
	if (DefaultAnimInstance)
	{
		GetMesh()->SetAnimClass(DefaultAnimInstance);
	}
	//메쉬 레플리케이트
	GetMesh()->SetIsReplicated(true);
	SetReplicates(true);
	SetReplicateMovement(true);

	// Physics Handle 컴포넌트 (왼손/오른손용)
	LeftHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("LeftPhysicsHandle"));
	RightHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("RightPhysicsHandle"));

	// Replication 설정 (데디 서버 고려)
	LeftHandle->SetIsReplicated(true);
	RightHandle->SetIsReplicated(true);

	//카메라 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	//Ragdoll
	RagdollComponent = CreateDefaultSubobject<UTT_RagdollComponent>(TEXT("RagdollComponent"));

	//physicalAnimation

	PhysicalAnimation = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));


	// Input.

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextRef(TEXT("/Game/TumbleTown/Input/IMC_Default.IMC_Default"));
	if (MappingContextRef.Object)
	{
		DefaultMappingContext = MappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_Move.IA_Move"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_Look.IA_Look"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}


	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionRef.Object)
	{
		JumpAction = JumpActionRef.Object;
	}


	static ConstructorHelpers::FObjectFinder<UInputAction> LeftHandActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_LeftHand.IA_LeftHand"));
	if (LeftHandActionRef.Object)
	{
		LeftHandAction = LeftHandActionRef.Object;

	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RightHandActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_RightHand.IA_RightHand"));
	if (RightHandActionRef.Object)
	{
		RightHandAction = RightHandActionRef.Object;

	}
	static ConstructorHelpers::FObjectFinder<UInputAction> ArmVerticalActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_ArmVertical.IA_ArmVertical"));
	if (ArmVerticalActionRef.Object)
	{
		ArmVerticalAction = ArmVerticalActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> StandUpActionRef(TEXT("/Game/TumbleTown/Input/Actions/IA_ArmVertical.IA_ArmVertical"));
	if (StandUpActionRef.Object)
	{
		ArmVerticalAction = StandUpActionRef.Object;
	}

}

void ATT_CharacterPlayer::BeginPlay()
{

	Super::BeginPlay();

	// Add InputMapping Context to Enhanced Input System.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	// [중요] 서버는 LocalPlayer가 존재하지 않기 때문에 GetLocalPlayer()는 nullptr를 반환함
	// 따라서 Client에서만 실행되도록 조건 추가
	// 초기 스폰 위치를 저장 (첫 리스폰 위치)

	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, [this]()
		{
			RagdollComponent->Server_SetRagdoll(
				GetMesh(), GetCapsuleComponent(), TEXT("Hips"),
				PhysicalAnimation, TEXT("RightFoot"), TEXT("LeftFoot"),
				GetCharacterMovement(), true
			);
		}, 0.2f, false);
	// 델리게이트 바인딩
}

void ATT_CharacterPlayer::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	if (HasAuthority())
	{
		if (LeftHandle->GrabbedComponent)
		{
			FVector LeftTargetLocation = GetMesh()->GetSocketLocation(TEXT("hand_l"));
			LeftHandle->SetTargetLocation(LeftTargetLocation);
		}

		if (RightHandle->GrabbedComponent)
		{
			FVector RightTargetLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
			RightHandle->SetTargetLocation(RightTargetLocation);
		}
	}

	if (HasAuthority() && GetActorLocation().Z < -1000.f && !bIsFallingToDeath)
	{
		bIsFallingToDeath = true;
		UE_LOG(LogTemp, Warning, TEXT("waitfor respawn"));

		RespawnAtCheckpoint();

		if (RagdollComponent)
		{
			RagdollComponent->Server_ActivateRagdoll();
		}
	}
}

void ATT_CharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// [공통 처리] - 첫 소유 시 Checkpoint 델리게이트 등
	if (HasAuthority())
	{
		// 최초 스폰이거나 리스폰일 때
		LastSavePoint = GetActorLocation();
		// 델리게이트 바인딩
		OnCheckpointUpdated.AddDynamic(this, &ATT_CharacterPlayer::UpdateCheckpoint);
	}

	// [클라이언트 전용 처리] - 인풋 매핑 재설정
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalController())
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					UE_LOG(LogTemp, Warning, TEXT("PossessedBy 입력 매핑 추가: %s"), *PC->GetName());
					SubSystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}
}

void ATT_CharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATT_CharacterPlayer, LastSavePoint);
	DOREPLIFETIME(ATT_CharacterPlayer, bIsLeftMouseHeld);
	DOREPLIFETIME(ATT_CharacterPlayer, bIsRightMouseHeld);
}
void ATT_CharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATT_CharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATT_CharacterPlayer::Look);

	//손설정

// 	EnhancedInputComponent->BindAction(ArmVerticalAction, ETriggerEvent::Triggered, this, &ATT_CharacterPlayer::ControlLeftArmVertical);
	//왼손
	EnhancedInputComponent->BindAction(LeftHandAction, ETriggerEvent::Started, this, &ATT_CharacterPlayer::OnLeftMousePressed);
	EnhancedInputComponent->BindAction(LeftHandAction, ETriggerEvent::Completed, this, &ATT_CharacterPlayer::OnLeftMouseReleased);
	//오른손
	EnhancedInputComponent->BindAction(RightHandAction, ETriggerEvent::Started, this, &ATT_CharacterPlayer::OnRightMousePressed);
	EnhancedInputComponent->BindAction(RightHandAction, ETriggerEvent::Completed, this, &ATT_CharacterPlayer::OnRightMouseReleased);

	//일어나기
	EnhancedInputComponent->BindAction(StandUpAction, ETriggerEvent::Completed, this, &ATT_CharacterPlayer::OnStandUpInput);


	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
}

void ATT_CharacterPlayer::Move(const FInputActionValue& Value)
{
	if (RagdollComponent && !RagdollComponent->bCanMove)
	{
		return; // Ragdoll 상태에선 이동 금지
	}

	// 입력 값 읽기.
	FVector2D Movement = Value.Get<FVector2D>();

	// 컨트롤러의 회전 값.
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// 방향 구하기.
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 무브먼트 컴포넌트에 값 전달.
	AddMovementInput(ForwardVector, Movement.X);
	AddMovementInput(RightVector, Movement.Y);
	// 	UE_LOG(LogTemp, Warning, TEXT(" Move Called: X = %f, Y = %f"), Movement.X, Movement.Y);


}

void ATT_CharacterPlayer::Look(const FInputActionValue& Value)
{

	// 입력 값 읽기.
	FVector2D LookVector = Value.Get<FVector2D>();

	// 회전량 (감도는 필요 시 곱하기 값 조정)
	float YawValue = LookVector.X * 5.0f;

	// 컨트롤러에 회전 추가.
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);

	// 캐릭터 자체 회전 (컨트롤러가 아닌 Actor를 직접 회전)
	AddActorLocalRotation(FRotator(0.f, YawValue, 0.f));

}

void ATT_CharacterPlayer::UpdateLeftLiftAnimation(bool bLifting)
{
	if (UTT_AnimInstance* Anim = Cast<UTT_AnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->SetLeftLifting(bLifting);
	}
}

void ATT_CharacterPlayer::UpdateRightLiftAnimation(bool bLifting)
{
	if (UTT_AnimInstance* Anim = Cast<UTT_AnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->SetRightLifting(bLifting);
	}

}


void ATT_CharacterPlayer::OnLeftMousePressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("왼쪽 마우스 눌림"));

	if (HasAuthority())
	{
		bIsLeftMouseHeld = true;
		UpdateLeftLiftAnimation(true);
		TryGrabObject(true);
	}

	else
	{
		Server_SetLeftMouseHeld(true);
	}

}

void ATT_CharacterPlayer::OnLeftMouseReleased(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		bIsLeftMouseHeld = false;
		UpdateLeftLiftAnimation(false);
		ServerReleaseObject(true);
	}
	else
	{
		Server_SetLeftMouseHeld(false);
	}
}

void ATT_CharacterPlayer::OnRightMousePressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("오른쪽 마우스 눌림"));

	if (HasAuthority())
	{
		bIsRightMouseHeld = true;
		UpdateRightLiftAnimation(true);
		TryGrabObject(false);
	}
	else
	{
		Server_SetRightMouseHeld(true);
	}
}

void ATT_CharacterPlayer::OnRightMouseReleased(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		bIsRightMouseHeld = false;
		UpdateRightLiftAnimation(false);
		ServerReleaseObject(false);
	}
	else
	{
		Server_SetRightMouseHeld(false);
	}
}

void ATT_CharacterPlayer::MulticastReleaseObject_Implementation(bool bIsLeft)
{
	UPhysicsHandleComponent* Handle = bIsLeft ? LeftHandle : RightHandle;
	if (Handle && Handle->GrabbedComponent)
	{
		Handle->ReleaseComponent();
	}
}

void ATT_CharacterPlayer::MulticastGrabObject_Implementation(bool bIsLeft, UPrimitiveComponent* TargetComp, FVector GrabLoc)
{
	if (!TargetComp) return;

	UPhysicsHandleComponent* Handle = bIsLeft ? LeftHandle : RightHandle;
	if (Handle)
	{
		Handle->GrabComponentAtLocationWithRotation(TargetComp, NAME_None, GrabLoc, TargetComp->GetComponentRotation());
		UE_LOG(LogTemp, Warning, TEXT("[MulticastGrabObject] 호출됨 - 컴포넌트: %s, 위치: %s"),
			*TargetComp->GetName(), *GrabLoc.ToString());
	}
}

void ATT_CharacterPlayer::ServerReleaseObject_Implementation(bool bIsLeft)
{
	UPhysicsHandleComponent* Handle = bIsLeft ? LeftHandle : RightHandle;
	if (Handle && Handle->GrabbedComponent)
	{
		Handle->ReleaseComponent();
		MulticastReleaseObject(bIsLeft);
	}
}

void ATT_CharacterPlayer::ServerGrabObject_Implementation(bool bIsLeft, UPrimitiveComponent* TargetComp)
{
	if (!TargetComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[TryGrab] HitComp가 null입니다."));
		return;
	}
	else if (!TargetComp->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryGrab] HitComp는 물리 시뮬레이션이 켜져있지 않습니다: %s"), *TargetComp->GetName());
	}
	FVector GrabLoc = bIsLeft ? GetMesh()->GetSocketLocation(TEXT("hand_l"))
		: GetMesh()->GetSocketLocation(TEXT("hand_r"));

	UPhysicsHandleComponent* Handle = bIsLeft ? LeftHandle : RightHandle;
	if (Handle)
	{
		Handle->GrabComponentAtLocationWithRotation(TargetComp, NAME_None, GrabLoc, TargetComp->GetComponentRotation());
		MulticastGrabObject(bIsLeft, TargetComp, GrabLoc);
		UE_LOG(LogTemp, Warning, TEXT("[ServerGrabObject] 호출됨: 손 = %s, 컴포넌트 = %s"),
			bIsLeft ? TEXT("왼손") : TEXT("오른손"),
			TargetComp ? *TargetComp->GetName() : TEXT("NULL"));
	}
}


void ATT_CharacterPlayer::Server_SetLeftMouseHeld_Implementation(bool bNewState)
{
	bIsLeftMouseHeld = bNewState;
}

void ATT_CharacterPlayer::Server_SetRightMouseHeld_Implementation(bool bNewState)
{
	bIsRightMouseHeld = bNewState;
}




void ATT_CharacterPlayer::OnStandUpInput()
{
	if (RagdollComponent && !RagdollComponent->bCanMove)
	{
		RagdollComponent->StandUp(); // 내부에서 서버 호출 처리
	}
}

void ATT_CharacterPlayer::UpdateCheckpoint(FVector NewSaveLocation)
{
	LastSavePoint = NewSaveLocation + FVector(0, 0, 500.f);;
	UE_LOG(LogTemp, Log, TEXT("SavePoint Update: %s"), *LastSavePoint.ToString());
}


/**
 * RespawnAtCheckpoint는 플레이어가 낙사한 후 일정 시간 뒤 저장된 위치로 리스폰시키는 함수입니다.
 *
 * 이 함수는 서버에서만 호출되며, 기존 Pawn을 제거한 뒤 GameMode를 통해 새 Pawn을 지정된 위치에 리스폰합니다.
 * 캐릭터가 맵 밖으로 떨어졌을 때 자동으로 이 함수가 실행되며, 항상 공중에서 리스폰되도록 오프셋을 포함합니다.
 */
void ATT_CharacterPlayer::RespawnAtCheckpoint()
{
	bIsFallingToDeath = false;

	FVector RespawnLoc = LastSavePoint.IsNearlyZero()
		? FVector(0, 0, 2000.f)  // 초기 리스폰 위치
		: LastSavePoint + FVector(0, 0, 100.f);  // 항상 공중에서 리스폰

	FRotator RespawnRot = FRotator::ZeroRotator;

	if (HasAuthority())
	{
		if (AController* PC = GetController())
		{
			// 기존 Pawn 제거 (중복 소유 방지)
			APawn* OldPawn = PC->GetPawn();
			if (OldPawn && OldPawn != this)
			{
				UE_LOG(LogTemp, Warning, TEXT(" 기존 Pawn 제거: %s"), *OldPawn->GetName());
				PC->UnPossess();
				OldPawn->Destroy();
			}

			if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
			{
				UE_LOG(LogTemp, Warning, TEXT(" RespawnLocation: %s"), *RespawnLoc.ToString());

				// GameMode를 통해 지정 위치로 리스폰
				GM->RestartPlayerAtTransform(PC, FTransform(RespawnRot, RespawnLoc));

				// 리스폰 결과 확인
				if (APawn* NewPawn = PC->GetPawn())
				{
					FVector CorrectedLocation = RespawnLoc + FVector(0.f, 0.f, 50.f);  // 50 정도 위로 띄움
					NewPawn->SetActorLocation(CorrectedLocation, false, nullptr, ETeleportType::TeleportPhysics);

					UE_LOG(LogTemp, Warning, TEXT("강제 위치 보정: %s"), *CorrectedLocation.ToString());
					UE_LOG(LogTemp, Warning, TEXT(" Respawn 완료: %s"), *NewPawn->GetName());
					UE_LOG(LogTemp, Warning, TEXT(" 위치: %s"), *NewPawn->GetActorLocation().ToString());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT(" Respawn 실패: Pawn 없음"));
				}
			}
		}
	}
}

void ATT_CharacterPlayer::TryGrabObject(bool bIsLeft)
{
	// 손 소켓 기준 시작점
	FVector Start = bIsLeft ? GetMesh()->GetSocketLocation(TEXT("hand_l"))
		: GetMesh()->GetSocketLocation(TEXT("hand_r"));

	FVector End = Start + GetActorForwardVector() * 150.f;

	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody); // Physics 오브젝트만 감지

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신 제외

	UE_LOG(LogTemp, Warning, TEXT("[TryGrab] 시작: 손 = %s"), bIsLeft ? TEXT("왼손") : TEXT("오른손"));
	UE_LOG(LogTemp, Warning, TEXT("Start: %s | End: %s"), *Start.ToString(), *End.ToString());

	// 트레이스 수행
	if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, Params))
	{
		UPrimitiveComponent* TargetComp = Hit.GetComponent();
		if (TargetComp && TargetComp->IsSimulatingPhysics())
		{
			ServerGrabObject(bIsLeft, TargetComp);

			// 시각 디버깅
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 2.f);
			UE_LOG(LogTemp, Warning, TEXT("[TryGrab] 트레이스 히트: %s"), *Hit.GetActor()->GetName());
		}
	}
}
