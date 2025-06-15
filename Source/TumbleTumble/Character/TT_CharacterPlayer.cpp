﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_CharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


#include "GameFrameWork/CharacterMovementComponent.h"
#include "../Game/TT_GameModeBase.h"

ATT_CharacterPlayer::ATT_CharacterPlayer()
{
	// 컨트롤러의 회전을 받아서 설정하는 모드를 모두 해제.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 무브먼트 설정.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	// 컴포넌트 설정.
	//GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 리소스 설정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn"));
	if (CharacterMesh.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// Animation Blueprint 설정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(TEXT("/Game/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn_C"));
	if (CharacterAnim.Class)
	{
		GetMesh()->SetAnimClass(CharacterAnim.Class);
	}



	//카메라 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera-> bUsePawnControlRotation = false;

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




}

void ATT_CharacterPlayer::BeginPlay()
{

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Controller: %s"), *GetController()->GetName());


	// Add InputMapping Context to Enhanced Input System.
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(DefaultMappingContext,0);
	}

	// 초기 스폰 위치를 저장 (첫 리스폰 위치)
	LastSavePoint = GetActorLocation();

	// 델리게이트 바인딩
	OnCheckpointUpdated.AddDynamic(this, &ATT_CharacterPlayer::UpdateCheckpoint);
}

void ATT_CharacterPlayer::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	if (HasAuthority() && GetActorLocation().Z < -1000.f && !bIsFallingToDeath)
	{
		bIsFallingToDeath = true;
		UE_LOG(LogTemp, Warning, TEXT("waitfor respawn"));

		RespawnAtCheckpoint();
	}
}

void ATT_CharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATT_CharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATT_CharacterPlayer::Look);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

}

void ATT_CharacterPlayer::Move(const FInputActionValue& Value)
{

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


}

void ATT_CharacterPlayer::Look(const FInputActionValue& Value)
{

	// 입력 값 읽기.
	FVector2D LookVector = Value.Get<FVector2D>();

	// 컨트롤러에 회전 추가.
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);

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