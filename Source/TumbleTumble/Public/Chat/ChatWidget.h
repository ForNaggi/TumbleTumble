#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat/ChatManager.h"
#include "ChatWidget.generated.h"

// 전방 선언
class UScrollBox;
class UEditableTextBox;
class UButton;
class UTextBlock;

 /**
 * 채팅 UI 위젯
 *
 * 주요 기능:
 * - ChatManager의 메세지를 UI에 표시합니다.
 * - 메시지를 ChatManager에 전달합니다.
 * - 클라이언트만 사용합니다.
 */
UCLASS()
class TUMBLETUMBLE_API UChatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 위젯 부품들 초기화.이벤트 등록. */
    virtual void NativeConstruct() override;

    /** 이벤트 해제 */
    virtual void NativeDestruct() override;

protected:


    /** 채팅 메시지 창 스크롤 */
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ChatScrollBox;

    /** 메시지 입력창 */
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MessageInputBox;

    /** 전송 버튼 */
    UPROPERTY(meta = (BindWidget))
    UButton* SendButton;

    /** 연결 상태 표시 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StatusText;

private:

    /** ChatManager */
    UPROPERTY()
    UChatManager* ChatManager;

    /** 이벤트가 바인딩되었는지 여부 */
    bool bEventsbound;


    /** 전송 버튼 클릭 */
    UFUNCTION()
    void OnSendButtonClicked();

    /** Enter 키 입력 */
    UFUNCTION()
    void OnMessageInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    /** 메시지 수신 */
    UFUNCTION()
    void OnMessageReceived(const FString& Message);


    // 내부 함수들

    /** ChatManager 가져오기 */
    void InitializeChatManager();

    /** 메시지를 채팅창에 추가 */
    void AddMessageToChat(const FString& Message, const FLinearColor& Color = FLinearColor::White);

    /** 메시지 전송 */
    void SendMessage();

    /** 상태 텍스트 업데이트 */
    void UpdateStatus();
};