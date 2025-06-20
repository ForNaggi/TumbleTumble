// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_GimIckBase.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "TT_DoorGimmick.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_DoorGimmick : public ATT_GimIckBase
{
	GENERATED_BODY()
	
public:
    ATT_DoorGimmick();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OpenDoor();

 /** 문 닫기 RPC (모든 클라이언트에게 동기화) */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_CloseDoor();

    UFUNCTION(BlueprintCallable)
    void OpenDoor();
	UFUNCTION(BlueprintCallable)
	void CloseDoor();



protected:
    virtual void BeginPlay() override;


    /** 매 프레임 문 위치를 선형 보간으로 조절하는 타이머 함수 */
  
  protected:
      // 메쉬 컴포넌트
   UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DoorMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DoorMeshOpposite;

	// 타임라인용 커브
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCurveFloat> MoveCurve;
    
    FTimeline DoorTimeline;

	// Timeline Callback
	UFUNCTION()
	void HandleTimelineProgress(float Value);    /** 타이머 핸들 */
    // 상태
    bool bIsOpening = false;

    // 기준 위치 저장
    FTransform StartTransformLeft;
    FTransform StartTransformRight;
};
