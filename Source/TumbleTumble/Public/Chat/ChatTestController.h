// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Chat/ChatManager.h"
#include "ChatTestController.generated.h"

/**
 * 
 */
UCLASS()
class TUMBLETUMBLE_API AChatTestController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    virtual void PlayerTick(float DeltaTime);

    UFUNCTION(Exec)
    void ChatConnect(const FString& IP = TEXT("127.0.0.1"), int32 Port = 5500);

    UFUNCTION(Exec)
    void ChatDisconnect();

    UFUNCTION(Exec)
    void ChatSend(const FString& Message);

private:
    UPROPERTY()
    UChatManager* SubsystemChatManager;

    // ChatManager 객체 주소를 가져옵니다.
    UChatManager* GetChatManager();
	
};
