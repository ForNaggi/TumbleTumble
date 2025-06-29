// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_GimIckBase.h"
#include "../Interface/TT_GimmickInteractable.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "TT_ButtonGimmick.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API ATT_ButtonGimmick : public ATT_GimIckBase, public ITT_GimmickInteractable
{
	GENERATED_BODY()
	
public:
    ATT_ButtonGimmick();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void Tick(float DeltaTime) override;
protected:
    virtual void BeginPlay() override;
 
UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
TObjectPtr<class UBoxComponent> OverlapVolume;  // 실제 트리거용

UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
TObjectPtr < class UStaticMeshComponent> ButtonBaseMesh;

UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
TObjectPtr < class UStaticMeshComponent> ButtonMesh;


    // 버튼에 연결된 문 목록
    UPROPERTY(EditAnywhere, Replicated)
    TArray<TObjectPtr<class ATT_DoorGimmick>> LinkedDoors;

    /** 현재 눌려있는 상태인지 */
    UPROPERTY(VisibleAnywhere)
    bool bIsPressed = false;

    // 버튼 위에 올라간 액터 수
    int32 OverlappingCount = 0;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	
    UFUNCTION()
    void CheckAndCloseDoor();
	UFUNCTION()
	void CheckAndOpenDoor();

//눌림
protected:
// Timeline 관련
UPROPERTY()
FTimeline PressTimeline;

UPROPERTY(EditAnywhere, Category = "Button|Curve")
UCurveFloat* PressCurve;

// 버튼 눌림 거리
UPROPERTY(EditAnywhere, Category = "Button|Visual")
float PressDistance = 11.0f;

// 초기 위치 저장용
FVector InitialButtonLocation;

// 타임라인 콜백 함수
UFUNCTION()
void HandlePressProgress(float Value);
};
