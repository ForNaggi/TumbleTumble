// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Chat/ChatManager.h"
#include "ChatTestController.generated.h"

// 전방 선언
class UChatWidget;

/**
 * 플레이어 컨트롤러
 *
 * 주요 기능:
 * - 서버 연결 시 생성되는 플레이어컨트롤러.
 * - 채팅 채널에 연결/해제 요청.
 * - 채팅에 필요한 UI 생성.
 * 
 * @note
 * 채팅 관련은 데디서버에서는 동작하지 않습니다.
 */
UCLASS()
class TUMBLETUMBLE_API AChatTestController : public APlayerController
{
    GENERATED_BODY()

public:
    /** UI 정보 담기 */
    AChatTestController();

public:
    /** 채팅 매니저를 가져와 변수로 저장. 채팅 서버와 연결. */
    virtual void BeginPlay() override;

    /** 채팅 서버 연결 종료*/
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Tick마다 호출 (채팅서버와 소통) */
    virtual void PlayerTick(float DeltaTime) override;

    /** 메시지 전송 (디버그) */
    UFUNCTION(Exec)
    void ChatSend(const FString& Message);

    /** Chat UI 서브 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
    TSubclassOf<UChatWidget> ChatWidgetClass;

protected:
    
    /** 채팅서버 IP 주소 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
    FString ChatServerIP = TEXT("127.0.0.1");

    /** 채팅서버 포트 번호 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
    int32 ChatServerPort = 5500;

private:

    /** ChatManager */
    UPROPERTY()
    UChatManager* ChatManager;

    /** 채팅 UI 위젯 */
    UPROPERTY()
    UChatWidget* ChatWidget;

    /** 채팅 시스템 활성화 여부 */
    bool bIsChatSystemEnabled;

    /** ChatManager 가져오기 */
    UChatManager* GetChatManager();

    /** 채팅 UI 생성 및 표시 */
    void CreateChatUI();

    /** 채팅서버 연결 */
    void ConnectToChat();

    /** 채팅서버 연결 해제 */
    void DisconnectFromChat();
};