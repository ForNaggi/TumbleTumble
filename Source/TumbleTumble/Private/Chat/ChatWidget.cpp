#include "Chat/ChatWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 데디케이티드 서버에서는 UI 생성 안함
    UWorld* World = this->GetWorld();
    if (World && World->GetNetMode() == NM_DedicatedServer)
    {
        UE_LOG(LogTemp, Log, TEXT("데디케이티드 서버는 채팅 UI 생성 안함"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("채팅 UI 초기화"));

    // 변수 초기화
    this->bEventsbound = false;

    // ChatManager 가져오기
    this->InitializeChatManager();

    // 버튼 이벤트 연결
    if (this->SendButton)
    {
        this->SendButton->OnClicked.AddDynamic(this, &UChatWidget::OnSendButtonClicked);
    }

    // 입력창 Enter 키 이벤트 연결
    if (this->MessageInputBox)
    {
        this->MessageInputBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnMessageInputCommitted);
    }

    // 메시지 수신 이벤트 연결
    if (this->ChatManager)
    {
        this->ChatManager->OnMessageReceived.AddDynamic(this, &UChatWidget::OnMessageReceived);
        this->bEventsbound = true;
    }

    // 초기 메시지 표시
    this->AddMessageToChat(TEXT("=== 채팅 시스템 ==="), FLinearColor::Yellow);
    this->AddMessageToChat(TEXT("서버에 자동으로 연결됩니다..."), FLinearColor::Yellow);

    // 상태 업데이트
    this->UpdateStatus();

    UE_LOG(LogTemp, Log, TEXT("채팅 UI 초기화 완료"));
}

void UChatWidget::NativeDestruct()
{
    UE_LOG(LogTemp, Log, TEXT("채팅 UI 해제"));

    // 이벤트 해제
    if (this->ChatManager && this->bEventsbound)
    {
        this->ChatManager->OnMessageReceived.RemoveDynamic(this, &UChatWidget::OnMessageReceived);
        this->bEventsbound = false;
    }

    Super::NativeDestruct();
}

void UChatWidget::OnSendButtonClicked()
{
    // 전송 버튼 클릭.
    UE_LOG(LogTemp, Log, TEXT("전송 버튼 클릭"));
    this->SendMessage();
}

void UChatWidget::OnMessageInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    // Enter 키로 입력된 경우만 처리.
    if (CommitMethod == ETextCommit::OnEnter)
    {
        UE_LOG(LogTemp, Log, TEXT("Enter 키로 메시지 입력"));
        this->SendMessage();
    }
}

void UChatWidget::OnMessageReceived(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("메시지 수신: %s"), *Message);

    // 채팅창에 메시지 추가.
    this->AddMessageToChat(Message, FLinearColor::White);

    // 상태 업데이트
    this->UpdateStatus();
}

void UChatWidget::InitializeChatManager()
{
    if (UGameInstance* GameInstance = this->GetGameInstance())
    {
        this->ChatManager = GameInstance->GetSubsystem<UChatManager>();

        if (this->ChatManager)
        {
            UE_LOG(LogTemp, Log, TEXT("ChatManager 연결 성공"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ChatManager를 찾을 수 없음"));
        }
    }
}

void UChatWidget::AddMessageToChat(const FString& Message, const FLinearColor& Color)
{
    if (this->ChatScrollBox == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("ChatScrollBox가 없음"));
        return ;
    }

    // 새 텍스트 블록 생성
    UTextBlock* NewTextBlock = NewObject<UTextBlock>(this);
    if (NewTextBlock == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("TextBlock 생성 실패"));
        return ;
    }

    // 텍스트 설정
    NewTextBlock->SetText(FText::FromString(Message));
    NewTextBlock->SetColorAndOpacity(FSlateColor(Color));

    // 폰트 크기 설정
    FSlateFontInfo FontInfo = NewTextBlock->GetFont();
    FontInfo.Size = 12;
    NewTextBlock->SetFont(FontInfo);

    // 채팅창에 추가
    this->ChatScrollBox->AddChild(NewTextBlock);

    // 스크롤을 맨 아래로
    this->ChatScrollBox->ScrollToEnd();
}

void UChatWidget::SendMessage()
{
    if (this->MessageInputBox == nullptr || this->ChatManager == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("입력창 또는 ChatManager가 없음"));
        return ;
    }

    // 입력된 메시지 가져오기
    FString InputMessage = MessageInputBox->GetText().ToString();

    // 빈 메시지 체크
    if (InputMessage.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("빈 메시지입니다"));
        return ;
    }

    // 연결 상태 확인
    if (this->ChatManager->GetConnectionState() != EChatConnectionState::Connected)
    {
        this->AddMessageToChat(TEXT("[오류] 서버에 연결되어 있지 않습니다"), FLinearColor::Red);
        return ;
    }

    // 메시지 전송
    bool bSuccess = this->ChatManager->SendMessage(InputMessage);

    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("메시지 전송 성공: %s"), *InputMessage);

        // 입력창 비우기
        this->MessageInputBox->SetText(FText::GetEmpty());

        //// 포커스 다시 설정
        //this->MessageInputBox->SetKeyboardFocus();
    }
    else
    {
        this->AddMessageToChat(TEXT("[오류] 메시지 전송 실패"), FLinearColor::Red);
        UE_LOG(LogTemp, Error, TEXT("메시지 전송 실패: %s"), *InputMessage);
    }

    // 상태 업데이트
    this->UpdateStatus();
}

void UChatWidget::UpdateStatus()
{
    if (this->StatusText == nullptr || this->ChatManager == nullptr)
    {
        return ;
    }

    // 연결 상태에 따라 텍스트 설정
    EChatConnectionState State = this->ChatManager->GetConnectionState();

    if (State == EChatConnectionState::Connected)
    {
        this->StatusText->SetText(FText::FromString(TEXT("연결됨")));
        this->StatusText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));

        // 입력창 활성화
        if (this->MessageInputBox)
        {
            this->MessageInputBox->SetIsEnabled(true);
            this->MessageInputBox->SetHintText(FText::FromString(TEXT("메시지를 입력하세요...")));
        }

        // 전송 버튼 활성화
        if (this->SendButton)
        {
            this->SendButton->SetIsEnabled(true);
        }
    }
    else
    {
        this->StatusText->SetText(FText::FromString(TEXT("연결 중...")));
        this->StatusText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));

        // 입력창 비활성화
        if (this->MessageInputBox)
        {
            this->MessageInputBox->SetIsEnabled(false);
            this->MessageInputBox->SetHintText(FText::FromString(TEXT("연결 중...")));
        }

        // 전송 버튼 비활성화
        if (this->SendButton)
        {
            this->SendButton->SetIsEnabled(false);
        }
    }
}