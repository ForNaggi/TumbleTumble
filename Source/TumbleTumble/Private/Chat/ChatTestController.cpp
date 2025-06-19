// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/ChatTestController.h"
#include "Engine/Engine.h"

void AChatTestController::BeginPlay()
{
    Super::BeginPlay();

    this->SubsystemChatManager = GetGameInstance()->GetSubsystem<UChatManager>();

    if (this->SubsystemChatManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController: ChatManager 연결 완료."));

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                TEXT("ChatManager 준비 완료, 콘솔에서 ChatConnect 사용 가능."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController: ChatManager 찾을 수 없음."));
    }
}

UChatManager* AChatTestController::GetChatManager()
{
    if (this->SubsystemChatManager == nullptr)
    {
        this->SubsystemChatManager = GetGameInstance()->GetSubsystem<UChatManager>();
    }
    return (this->SubsystemChatManager);
}

void AChatTestController::ChatConnect(const FString& IP, int32 Port)
{
    UChatManager* ChatManager = this->GetChatManager();
    if (ChatManager == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("[Exec] ChatManager 없음."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Exec] ChatConnect 실행: %s:%d."), *IP, Port);

    bool bSuccess = ChatManager->ConnectToServer(IP, Port);

    FString Message = bSuccess ?
        FString::Printf(TEXT("[Chat] 연결 시도: %s:%d"), *IP, Port) :
        FString::Printf(TEXT("[Chat] 연결 실패: %s:%d"), *IP, Port);

    FColor Color = bSuccess ? FColor::Green : FColor::Red;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, Color, Message);
    }
}

void AChatTestController::ChatDisconnect()
{
    UChatManager* ChatManager = this->GetChatManager();
    if (ChatManager == nullptr)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Exec] ChatDisconnect 실행."));

    ChatManager->DisconnectFromServer();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange,
            TEXT("[Chat] 연결 해제됨."));
    }
}

void AChatTestController::ChatSend(const FString& Message)
{
    UChatManager* ChatManager = this->GetChatManager();
    if (ChatManager == nullptr)
    {
        return ;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Exec] ChatSend 실행: %s."), *Message);

    if (Message.IsEmpty())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
                TEXT("[Chat] 메시지를 입력하세요! 예: ChatSend Hello"));
        }
        return ;
    }

    bool bSuccess = ChatManager->SendMessage(Message);

    FString DisplayMessage = bSuccess ?
        FString::Printf(TEXT("[Chat] 전송: %s."), *Message) :
        TEXT("[Chat] 전송 실패 - 연결을 확인하세요.");

    FColor Color = bSuccess ? FColor::Cyan : FColor::Red;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, Color, DisplayMessage);
    }
}
