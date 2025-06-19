#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Delegates/Delegate.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "ChatManager.generated.h"

/** 채팅 서버 연결 상태 */
UENUM(BlueprintType)
enum class EChatConnectionState : uint8
{
    Disconnected    UMETA(DisplayName = "연결 안됨", ToolTip = "서버와의 연결이 끊어짐"),
    Connected       UMETA(DisplayName = "연결됨", ToolTip = "서버와의 연결됨"),
    Error           UMETA(DisplayName = "오류", ToolTip = "서버와의 연결간 오류가 발생할 경우")
};

/**
 * 채팅 매니저 - TCP 서버와 채팅하는 클래스 (싱글톤)
 *
 * 주요 기능:
 * - 서버 연결/해제 (ConnectToServer, DisconnectFromServer)
 * - 메시지 전송/수신 (SendMessage, 자동 수신)
 * - UTF-8 한글 지원
 */
UCLASS()
class TUMBLETUMBLE_API UChatManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    
    /** 내부 변수들을 초기화합니다. 플렛폼에 맞는 소켓시스템을 설정합니다. */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    /** 내부 변수들을 정리합니다. ChatServer와의 연결을 해제합니다. */
    virtual void Deinitialize() override;

    /**
     * 서버에 연결
     * @param ServerIP 서버 IP 주소 (기본: 127.0.0.1).
     * @param ServerPort 서버 포트 (기본: 5500).
     * @return 연결 성공하면 true, 실패 시 false.
     */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    bool ConnectToServer(const FString& ServerIP = TEXT("127.0.0.1"), int32 ServerPort = 5500);

    /** 서버 연결 해제합니다. */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void DisconnectFromServer();

    /**
     * 메시지 전송 (UTF-8 한글 지원).
     * @param Message 보낼 메시지.
     * @return 전송 성공하면 true, 실패 시 false.
     */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    bool SendMessage(const FString& Message);

    /** 현재 연결 상태 확인 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Chat")
    EChatConnectionState GetConnectionState() const { return ConnectionState; }

    /** 메세지 수신 시 알림 전달(메세지 포함) 델리게이트 */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Message);

    UPROPERTY(BlueprintAssignable, Category = "Chat")
    FOnMessageReceived OnMessageReceived;

private:

    // 소켓 관련
    /** 채팅용 소켓 */
    FSocket* ChatSocket;
    /** 소켓 시스템(Window용으로 호출될 예정) */
    ISocketSubsystem* SocketSubsystem;      

    // 상태 관리
    /** 현재 연결 상태 */
    EChatConnectionState ConnectionState;   

    // 메시지 수신용
    /** 수신 버퍼 */
    TArray<uint8> ReceiveBuffer;
    /** 부분 메시지 저장 */
    FString PartialMessage;                 

    // 상수들
    /** 수신 버퍼 크기 */
    static const int32 BUFFER_SIZE = 1024;  

    /** 메시지 수신 확인 */
    void CheckForMessages();

    /** 실제 데이터 수신 처리 */
    void ReceiveData();

    /** UTF-8 바이트를 FString으로 변환 (한글 지원) */
    FString BytesToString(const uint8* Data, int32 Length);

    /** FString을 UTF-8 바이트로 변환 (한글 지원) */
    TArray<uint8> StringToBytes(const FString& String);
};