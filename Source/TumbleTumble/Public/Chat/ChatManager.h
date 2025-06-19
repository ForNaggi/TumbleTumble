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

/** ä�� ���� ���� ���� */
UENUM(BlueprintType)
enum class EChatConnectionState : uint8
{
    Disconnected    UMETA(DisplayName = "���� �ȵ�", ToolTip = "�������� ������ ������"),
    Connected       UMETA(DisplayName = "�����", ToolTip = "�������� �����"),
    Error           UMETA(DisplayName = "����", ToolTip = "�������� ���ᰣ ������ �߻��� ���")
};

/**
 * ä�� �Ŵ��� - TCP ������ ä���ϴ� Ŭ���� (�̱���)
 *
 * �ֿ� ���:
 * - ���� ����/���� (ConnectToServer, DisconnectFromServer)
 * - �޽��� ����/���� (SendMessage, �ڵ� ����)
 * - UTF-8 �ѱ� ����
 */
UCLASS()
class TUMBLETUMBLE_API UChatManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    
    /** ���� �������� �ʱ�ȭ�մϴ�. �÷����� �´� ���Ͻý����� �����մϴ�. */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    /** ���� �������� �����մϴ�. ChatServer���� ������ �����մϴ�. */
    virtual void Deinitialize() override;

    /**
     * ������ ����
     * @param ServerIP ���� IP �ּ� (�⺻: 127.0.0.1).
     * @param ServerPort ���� ��Ʈ (�⺻: 5500).
     * @return ���� �����ϸ� true, ���� �� false.
     */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    bool ConnectToServer(const FString& ServerIP = TEXT("127.0.0.1"), int32 ServerPort = 5500);

    /** ���� ���� �����մϴ�. */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void DisconnectFromServer();

    /**
     * �޽��� ���� (UTF-8 �ѱ� ����).
     * @param Message ���� �޽���.
     * @return ���� �����ϸ� true, ���� �� false.
     */
    UFUNCTION(BlueprintCallable, Category = "Chat")
    bool SendMessage(const FString& Message);

    /** ���� ���� ���� Ȯ�� */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Chat")
    EChatConnectionState GetConnectionState() const { return ConnectionState; }

    /** �޼��� ���� �� �˸� ����(�޼��� ����) ��������Ʈ */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Message);

    UPROPERTY(BlueprintAssignable, Category = "Chat")
    FOnMessageReceived OnMessageReceived;

private:

    // ���� ����
    /** ä�ÿ� ���� */
    FSocket* ChatSocket;
    /** ���� �ý���(Window������ ȣ��� ����) */
    ISocketSubsystem* SocketSubsystem;      

    // ���� ����
    /** ���� ���� ���� */
    EChatConnectionState ConnectionState;   

    // �޽��� ���ſ�
    /** ���� ���� */
    TArray<uint8> ReceiveBuffer;
    /** �κ� �޽��� ���� */
    FString PartialMessage;                 

    // �����
    /** ���� ���� ũ�� */
    static const int32 BUFFER_SIZE = 1024;  

    /** �޽��� ���� Ȯ�� */
    void CheckForMessages();

    /** ���� ������ ���� ó�� */
    void ReceiveData();

    /** UTF-8 ����Ʈ�� FString���� ��ȯ (�ѱ� ����) */
    FString BytesToString(const uint8* Data, int32 Length);

    /** FString�� UTF-8 ����Ʈ�� ��ȯ (�ѱ� ����) */
    TArray<uint8> StringToBytes(const FString& String);
};