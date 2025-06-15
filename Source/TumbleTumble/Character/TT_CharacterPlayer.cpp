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

ATT_CharacterPlayer::ATT_CharacterPlayer()
{
	// ��Ʈ�ѷ��� ȸ���� �޾Ƽ� �����ϴ� ��带 ��� ����.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// �����Ʈ ����.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	// ������Ʈ ����.
	//GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// ���ҽ� ����.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn"));
	if (CharacterMesh.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// Animation Blueprint ����.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(TEXT("/Game/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn_C"));
	if (CharacterAnim.Class)
	{
		GetMesh()->SetAnimClass(CharacterAnim.Class);
	}



	//ī�޶� ����
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

	// �ʱ� ���� ��ġ�� ���� (ù ������ ��ġ)
	LastSavePoint = GetActorLocation();

	// ��������Ʈ ���ε�
	OnCheckpointUpdated.AddDynamic(this, &ATT_CharacterPlayer::UpdateCheckpoint);
}

void ATT_CharacterPlayer::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	if (HasAuthority() && GetActorLocation().Z < -1000.f && !bIsFallingToDeath)
	{
		bIsFallingToDeath = true;
		UE_LOG(LogTemp, Warning, TEXT("waitfor respawn"));

		// 2�� �� ������
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ATT_CharacterPlayer::RespawnAtCheckpoint, 1.0f, false);
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

	// �Է� �� �б�.
	FVector2D Movement = Value.Get<FVector2D>();

	// ��Ʈ�ѷ��� ȸ�� ��.
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// ���� ���ϱ�.
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// �����Ʈ ������Ʈ�� �� ����.
	AddMovementInput(ForwardVector, Movement.X);
	AddMovementInput(RightVector, Movement.Y);


}

void ATT_CharacterPlayer::Look(const FInputActionValue& Value)
{

	// �Է� �� �б�.
	FVector2D LookVector = Value.Get<FVector2D>();

	// ��Ʈ�ѷ��� ȸ�� �߰�.
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);

}

void ATT_CharacterPlayer::UpdateCheckpoint(FVector NewSaveLocation)
{
	LastSavePoint = NewSaveLocation + FVector(0, 0, 500.f);;
	UE_LOG(LogTemp, Log, TEXT("SavePoint Update: %s"), *LastSavePoint.ToString());
}


void ATT_CharacterPlayer::RespawnAtCheckpoint()
{
	bIsFallingToDeath = false;

	FVector RespawnLoc = LastSavePoint.IsNearlyZero()
		? FVector(0, 0, 2000.f) // ����Ʈ ��ġ
		: LastSavePoint;

	FRotator RespawnRot = FRotator::ZeroRotator;


	if (AController* PC = GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("RespawnLocation: %s"), *RespawnLoc.ToString());

		// ���������� ȣ��ǹǷ� AuthGameMode ���
		if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
		{
			GM->RestartPlayerAtTransform(PC, FTransform(RespawnRot, RespawnLoc));
		}
	}
}
