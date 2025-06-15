// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACheckPointBase.h"
#include "ACheckpointZone.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API AACheckpointZone : public AACheckPointBase
{
	GENERATED_BODY()
	
public:
    /**
     * ACheckpointZone는 플레이어가 이 영역에 진입했을 때
     * 해당 위치를 세이브 포인트로 저장하는 역할을 하는 액터입니다.
     * APropBase를 상속하여 공통 트리거 로직을 재사용합니다.
     */
    AACheckpointZone();

protected:
    /**
     * SetupTrigger는 트리거 박스에 오버랩 이벤트를 바인딩합니다.
     * 부모 클래스의 SetupTrigger를 오버라이드하여 실행됩니다.
     * 이 함수는 BeginPlay 시 자동으로 호출됩니다.
     */
    virtual void SetupTrigger() override;

    /**
     * OnTriggerEntered는 플레이어가 트리거 박스에 진입했을 때 호출됩니다.
     * @param OverlappedComp - 오버랩된 트리거 컴포넌트
     * @param OtherActor - 트리거에 진입한 액터 (플레이어)
     * @param OtherComp - 액터의 충돌 컴포넌트
     * @param OtherBodyIndex - 오버랩 인덱스
     * @param bFromSweep - 스윕으로 발생했는지 여부
     * @param SweepResult - 스윕 결과 정보
     *
     * 이 함수는 세이브 지점 기능 구현에 사용됩니다.
     */
    UFUNCTION()
    void OnTriggerEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	
};
