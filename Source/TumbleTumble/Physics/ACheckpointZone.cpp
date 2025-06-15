// Fill out your copyright notice in the Description page of Project Settings.


#include "ACheckpointZone.h"
#include "../Character/TT_CharacterPlayer.h"
#include "Components/BoxComponent.h"
#include "ACheckpointZone.h"

/**
 * ACheckpointZone 생성자입니다.
 * 트리거 박스는 부모 클래스에서 생성되므로 별도 로직은 필요하지 않습니다.
 * 필요한 경우 추가 컴포넌트를 이 생성자에서 붙일 수 있습니다.
 */
AACheckpointZone::AACheckpointZone()
{
    // 생성자 내용은 기본이면 생략 가능
}

/**
 * SetupTrigger 함수는 트리거 박스의 오버랩 이벤트에
 * OnTriggerEntered를 바인딩하여 플레이어 진입 시 이벤트가 발생하도록 합니다.
 * BeginPlay 시 자동으로 호출됩니다.
 */
void AACheckpointZone::SetupTrigger()
{
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AACheckpointZone::OnTriggerEntered);
    }
}
/**
 * OnTriggerEntered는 플레이어가 이 체크포인트의 트리거 영역에 진입했을 때 호출됩니다.
 * @param OverlappedComp - 충돌 감지된 트리거 박스
 * @param OtherActor - 진입한 액터 (보통 플레이어 캐릭터)
 * @param OtherComp - 해당 액터의 충돌 컴포넌트
 * @param OtherBodyIndex - 오버랩 인덱스
 * @param bFromSweep - 스윕에 의해 발생한 경우 true
 * @param SweepResult - 스윕 결과
 *
 * 이 함수는 캐릭터의 LastSavePoint를 현재 위치로 갱신하여,
 * 낙사하거나 리스폰 시 이 위치에서 떨어지도록 합니다.
 */

void AACheckpointZone::OnTriggerEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ATT_CharacterPlayer* Character = Cast<ATT_CharacterPlayer>(OtherActor))
    {
        FVector NewSave = GetActorLocation() + FVector(0, 0, 500.f);  
        Character->OnCheckpointUpdated.Broadcast(NewSave); // 델리게이트 호출
        UE_LOG(LogTemp, Warning, TEXT("checkpoint saved: %s"), *Character->GetName());
    }
}