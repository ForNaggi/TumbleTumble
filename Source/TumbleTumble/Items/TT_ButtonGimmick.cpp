// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_ButtonGimmick.h"
#include "Net/UnrealNetwork.h"
#include "TT_DoorGimmick.h"
#include "Components/BoxComponent.h"

ATT_ButtonGimmick::ATT_ButtonGimmick()
{
    bReplicates = true;


    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    ButtonBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonBaseMesh"));
    ButtonBaseMesh->SetupAttachment(RootComponent);

    MainCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollider"));
    MainCollider->SetupAttachment(RootComponent);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(MainCollider);

    OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
    OverlapVolume->SetupAttachment(RootComponent);
    OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    OverlapVolume->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
    OverlapVolume->SetGenerateOverlapEvents(true);
}

void ATT_ButtonGimmick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATT_ButtonGimmick, LinkedDoors);


}
void ATT_ButtonGimmick::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PressTimeline.TickTimeline(DeltaTime);
}
void ATT_ButtonGimmick::BeginPlay()
{
    if (HasAuthority()) // 서버에서만 오버랩 바인딩
    {
        MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ATT_ButtonGimmick::OnBeginOverlap);
        MainCollider->OnComponentEndOverlap.AddDynamic(this, &ATT_ButtonGimmick::OnEndOverlap);
    }
    // 초기 위치 저장
    InitialButtonLocation = ButtonMesh->GetRelativeLocation();

    // 타임라인 바인딩
    if (PressCurve)
    {
        FOnTimelineFloat ProgressFunc;
        ProgressFunc.BindUFunction(this, FName("HandlePressProgress"));

        PressTimeline.AddInterpFloat(PressCurve, ProgressFunc);
        PressTimeline.SetLooping(false);
    }
}

void ATT_ButtonGimmick::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    CheckAndOpenDoor();
}

void ATT_ButtonGimmick::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CheckAndCloseDoor();
}

void ATT_ButtonGimmick::CheckAndCloseDoor()
{
    if (!HasAuthority()) return;

    TArray<AActor*> OverlappingActors;
    OverlapVolume->GetOverlappingActors(OverlappingActors);

    if (OverlappingActors.Num() == 0)
    {
        bIsPressed = false;

        for (ATT_DoorGimmick* Door : LinkedDoors)
        {
            if (Door)
            {
                Door->Multicast_CloseDoor(); // 모든 클라이언트에 문 닫기 지시
            }
        }
        // 버튼 복원 애니메이션
        PressTimeline.ReverseFromEnd();
    }
}

void ATT_ButtonGimmick::CheckAndOpenDoor()
{
    if (!HasAuthority()) return;

    TArray<AActor*> OverlappingActors;
    OverlapVolume->GetOverlappingActors(OverlappingActors);

    if (OverlappingActors.Num() > 0 && !bIsPressed)
    {
        bIsPressed = true;

        for (ATT_DoorGimmick* Door : LinkedDoors)
        {
            if (Door)
            {
                Door->Multicast_OpenDoor(); // 모든 클라이언트에 문 열기 지시
            }
        }
        // 버튼 눌림 애니메이션 시작
        PressTimeline.PlayFromStart();
    }
}

void ATT_ButtonGimmick::HandlePressProgress(float Value)
{
    FVector Offset = FVector(0.f, 0.f, -PressDistance * Value);
    ButtonMesh->SetRelativeLocation(InitialButtonLocation + Offset);
}
