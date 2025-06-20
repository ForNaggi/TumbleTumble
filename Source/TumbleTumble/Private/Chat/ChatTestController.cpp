// Fill out your copyright notice in the Description page of Project Settings.

#include "Chat/ChatTestController.h"
#include "Chat/ChatWidget.h" 
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"

AChatTestController::AChatTestController()
{
	ConstructorHelpers::FClassFinder<UChatWidget> ChatWidgetRef(TEXT("/Game/sounchoi/WBP_ChatWidget.WBP_ChatWidget_C"));

	if (ChatWidgetRef.Succeeded())
	{
		this->ChatWidgetClass = ChatWidgetRef.Class;
	}

	this->SetShowMouseCursor(true);
}

void AChatTestController::BeginPlay()
{
	Super::BeginPlay();

	// 데디서버는 작동하지 않음.
	if (this->HasAuthority() == true && this->IsLocalController() == false)
	{
		UE_LOG(LogTemp, Log, TEXT("데디케이티드 서버는 하지 않음."));
		return ;
	}

	// 변수 초기화.
	this->ChatManager = nullptr;
	this->ChatWidget = nullptr;

	// ChatManager 가져오기.
	this->ChatManager = this->GetGameInstance()->GetSubsystem<UChatManager>();
	if (this->ChatManager == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ChatManager를 찾을 수 없습니다."));
		return ;
	}

	UE_LOG(LogTemp, Log, TEXT("채팅 시스템 초기화 완료."));

	// 채팅 UI 생성 및 표시.
	this->CreateChatUI();

	// 안내 메시지
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("채팅 시스템 준비 완료."));
	}

	// 데디케이티드 서버 접속 완료 - 채팅서버 연결
	this->ConnectToChat();
}

void AChatTestController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 데디서버는 작동하지 않음.
	if (this->HasAuthority() == true && this->IsLocalController() == false)
	{
		Super::EndPlay(EndPlayReason);
		return ;
	}

	UE_LOG(LogTemp, Log, TEXT("게임 종료 - 채팅 연결 해제"));

	// 채팅서버 연결 해제
	this->DisconnectFromChat();

	Super::EndPlay(EndPlayReason);
}

void AChatTestController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// 데디서버는 작동하지 않음.
	if (this->HasAuthority() == true && this->IsLocalController() == false)
	{
		return;
	}

	// 메시지 수신 체크
	if (this->ChatManager && this->ChatManager->GetConnectionState() == EChatConnectionState::Connected)
	{
		this->ChatManager->CheckForMessages();
	}
}

void AChatTestController::ChatSend(const FString& Message)
{
	// 데디서버는 작동하지 않음.
	if (this->HasAuthority() == true && this->IsLocalController() == false)
	{
		return;
	}

	if (this->ChatManager == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ChatManager가 없습니다"));
		return ;
	}

	if (Message.IsEmpty())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("사용법: ChatSend 메세지"));
		}
		return;
	}

	// 연결 상태 확인
	if (this->ChatManager->GetConnectionState() != EChatConnectionState::Connected)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("채팅서버에 연결되어 있지 않습니다"));
		}
		return ;
	}

	// 메시지 전송
	bool bSuccess = this->ChatManager->SendMessage(Message);

	if (GEngine)
	{
		FColor Color = bSuccess ? FColor::Green : FColor::Red;
		FString ResultMessage = bSuccess ? FString::Printf(TEXT("메시지 전송: %s"), *Message) : TEXT("메시지 전송 실패");
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, Color, ResultMessage);
	}
}

UChatManager* AChatTestController::GetChatManager()
{
	// chatManager가 없다면 서브시스템에서 다시 가져옵니다.
	if (this->ChatManager == nullptr)
	{
		this->ChatManager = GetGameInstance()->GetSubsystem<UChatManager>();
	}
	return (this->ChatManager);
}

void AChatTestController::CreateChatUI()
{
	UE_LOG(LogTemp, Log, TEXT("채팅 UI 생성"));

	// 이미 생성되어 있으면 무시
	if (this->ChatWidget)
	{
		return ;
	}

	this->ChatWidget = CreateWidget<UChatWidget>(this->GetWorld(), this->ChatWidgetClass);

	if (this->ChatWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("채팅 UI 생성 실패!"));
		return ;
	}

	// 화면에 표시
	this->ChatWidget->AddToViewport();

	UE_LOG(LogTemp, Log, TEXT("채팅 UI 생성 완료"));
}

void AChatTestController::ConnectToChat()
{
	UE_LOG(LogTemp, Error, TEXT("여기까지는 옴"));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("여기까지는 옴"));

	if (this->ChatManager == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ChatManager가 없어서 연결 실패"));
		return;
	}

	// 이미 연결되어 있으면 무시
	if (this->ChatManager->GetConnectionState() == EChatConnectionState::Connected)
	{
		UE_LOG(LogTemp, Log, TEXT("이미 채팅서버에 연결되어 있음"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("채팅서버 연결 시도: %s:%d"), *(this->ChatServerIP), this->ChatServerPort);

	bool bSuccess = this->ChatManager->ConnectToServer(this->ChatServerIP, this->ChatServerPort);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("채팅서버 연결 성공!"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("채팅서버 연결 성공!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("채팅서버 연결 실패"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("채팅서버 연결 실패"));
		}
	}
}

void AChatTestController::DisconnectFromChat()
{
	if (this->ChatManager == nullptr)
	{
		return ;
	}

	if (this->ChatManager->GetConnectionState() == EChatConnectionState::Connected)
	{
		this->ChatManager->DisconnectFromServer();
		UE_LOG(LogTemp, Log, TEXT("채팅서버 연결 해제"));
	}
}
