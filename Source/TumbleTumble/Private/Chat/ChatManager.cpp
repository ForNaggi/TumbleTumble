#include "Chat/ChatManager.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogChatManager, Log, All);

void UChatManager::Initialize(FSubsystemCollectionBase& Collection)
{
    // 서브 시스템은 gameinstance 생성 후 리플렉션을 통해서 생성되고 initalize가 호출됩니다.
    Super::Initialize(Collection);

    // 변수들 초기화
    this->ChatSocket = nullptr;
    this->ConnectionState = EChatConnectionState::Disconnected;
    this->ReceiveBuffer.SetNum(BUFFER_SIZE);
    this->PartialMessage.Empty();

    // 소켓 시스템 가져오기.
    // 내부적으로 플렛폼에 맞는 ISocketSubsystem을 가져옵니다. 동적으로 해당 모듈을 로드한다.
    // IModuleInterface* NewModule = LoadSubsystemModule(SubsystemName.ToString());
    this->SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    if (this->SocketSubsystem)
    {
        UE_LOG(LogChatManager, Log, TEXT("ChatManager 초기화 완료."));
    }
    else
    {
        UE_LOG(LogChatManager, Error, TEXT("소켓 시스템을 찾을 수 없음."));
    }
}

void UChatManager::Deinitialize()
{
    // 연결되어 있으면 해제합니다.
    this->DisconnectFromServer();
    UE_LOG(LogChatManager, Log, TEXT("ChatManager 해제 완료."));

    // 부모 연결 해체.
    Super::Deinitialize();
}

bool UChatManager::ConnectToServer(const FString& ServerIP, int32 ServerPort)
{
    // 이미 연결되어 있으면 먼저 해제
    if (this->ConnectionState == EChatConnectionState::Connected)
    {
        UE_LOG(LogChatManager, Warning, TEXT("기존 연결 해제 후 새로 연결."));
        this->DisconnectFromServer();
    }
    
    // 소켓 시스템 확인
    if (this->SocketSubsystem == nullptr)
    {
        UE_LOG(LogChatManager, Error, TEXT("소켓 시스템이 없음."));
        this->ConnectionState = EChatConnectionState::Error;
        return (false);
    }

    UE_LOG(LogChatManager, Log, TEXT("서버 연결 시도: %s:%d"), *ServerIP, ServerPort);

    // 1. TCP 소켓 생성
    // CreateSocket (소켓 타입, 소켓 이름, 소켓의 NonBlocking 여부).
    this->ChatSocket = this->SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ChatSocket"), false);
    if (this->ChatSocket == nullptr)
    {
        UE_LOG(LogChatManager, Error, TEXT("소켓 생성 실패."));
        this->ConnectionState = EChatConnectionState::Error;
        return (false);
    }

    // 2. NonBlocking 모드 설정 (게임이 멈추지 않게)
    // #define FIONBIO _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
    this->ChatSocket->SetNonBlocking(true);

    // 3. IP 주소 변환
    // IPv4 주소는 32비트 숫자 형태로, 주로 a.b.c.d 형식의 문자열로 표현
    FIPv4Address IPv4Address;
    // 문자열의 a.b.c.d 형식으로 저장합니다.
    if (FIPv4Address::Parse(ServerIP, IPv4Address) == false)
    {
        UE_LOG(LogChatManager, Error, TEXT("잘못된 IP 주소: %s"), *ServerIP);
        this->DisconnectFromServer();
        return (false);
    }

    // 4. 서버 주소 설정
    // IPv4 IPv6 등 주소를 처리하는 클래스를 생성합니다.
    // provider 패턴이 적용되어 있다.
    TSharedRef<FInternetAddr> ServerAddress = this->SocketSubsystem->CreateInternetAddr();
    ServerAddress->SetIp(IPv4Address.Value);
    ServerAddress->SetPort(ServerPort);

    // 5. 서버에 연결
    // int32 Return = connect(Socket, (const sockaddr*)&(BSDAddr.Addr), BSDAddr.GetStorageSize());
    // 결국 내부적으로 winsock2를 사용하고 있음.
    if (this->ChatSocket->Connect(*ServerAddress))
    {
        // 연결 성공.
        this->ConnectionState = EChatConnectionState::Connected;
        UE_LOG(LogChatManager, Log, TEXT("서버 연결 성공."));
        return (true);
    }
    else
    {
        // 연결 실패.
        UE_LOG(LogChatManager, Warning, TEXT("서버 연결 실패."));
        this->DisconnectFromServer();
        return (false);
    }
}

void UChatManager::DisconnectFromServer()
{
    // 소켓 정리
    if (this->ChatSocket)
    {
        // 내부적으로는 플렛폼에 맞게 소켓을 닫고 있음.
        this->ChatSocket->Close();
        // 소켓을 할당 해제함.
        this->SocketSubsystem->DestroySocket(this->ChatSocket);
        this->ChatSocket = nullptr;
        UE_LOG(LogChatManager, Log, TEXT("소켓 해제."));
    }

    // 상태 변경
    this->ConnectionState = EChatConnectionState::Disconnected;
    // 메세지 정리
    this->PartialMessage.Empty();

    UE_LOG(LogChatManager, Log, TEXT("서버 연결 해제."));
}

bool UChatManager::SendMessage(const FString& Message)
{
    // 연결 상태 확인
    if (this->ConnectionState != EChatConnectionState::Connected || this->ChatSocket == nullptr)
    {
        UE_LOG(LogChatManager, Warning, TEXT("서버에 연결되지 않음."));
        return (false);
    }

    // 메세지 확인.
    if (Message.IsEmpty())
    {
        UE_LOG(LogChatManager, Warning, TEXT("빈 메시지는 전송할 수 없음."));
        return (false);
    }

    // UTF-8 바이트로 변환
    TArray<uint8> SendData = this->StringToBytes(Message);

    if (SendData.Num() == 0)
    {
        UE_LOG(LogChatManager, Error, TEXT("메시지 변환 실패: %s"), *Message);
        return (false);
    }

    // 서버로 전송 - 전송 바이트.
    int32 BytesSent = 0;

    if (this->ChatSocket->Send(SendData.GetData(), SendData.Num(), BytesSent) == true)
    {
        UE_LOG(LogChatManager, Log, TEXT("메시지 전송 성공: %s"), *Message);
        return (true);
    }
    else
    {
        UE_LOG(LogChatManager, Error, TEXT("메시지 전송 실패: %s"), *Message);
        return (false);
    }
}

void UChatManager::CheckForMessages()
{
    // 연결 상태 확인
    if (this->ConnectionState != EChatConnectionState::Connected || this->ChatSocket == nullptr)
    {
        return ;
    }

    // 소켓 연결 상태 확인
    if (this->ChatSocket->GetConnectionState() != SCS_Connected)
    {
        UE_LOG(LogChatManager, Warning, TEXT("소켓 연결이 끊어짐"));
        this->DisconnectFromServer();
        return ;
    }

    // 메시지 수신 시도
    this->ReceiveData();
}

void UChatManager::ReceiveData()
{
    if (this->ChatSocket == nullptr)
    {
        // 소켓 시스템이 없습니다.
        return ;
    }

    // 받을 데이터가 있는지 확인
    uint32 PendingDataSize = 0;
    if (this->ChatSocket->HasPendingData(PendingDataSize) == false || PendingDataSize == 0)
    {
        // 받을 데이터 없음
        return ;  
    }

    // 데이터 수신
    int32 BytesRead = 0;
    if (this->ChatSocket->Recv(ReceiveBuffer.GetData(), BUFFER_SIZE - 1, BytesRead) == false || BytesRead <= 0)
    {
        // 수신 실패
        return ;  
    }

    // UTF-8 바이트를 문자열로 변환
    FString ReceivedText = this->BytesToString(ReceiveBuffer.GetData(), BytesRead);
    if (ReceivedText.IsEmpty())
    {
        return ;
    }

    // 이전 부분 메시지와 합치기
    this->PartialMessage += ReceivedText;

    // 완전한 메시지들 찾기 (\r\n으로 구분)
    TArray<FString> Lines;

     //구분자로 나눈 문자열을 문자열 배열의 요소들로 분리합니다. split기능.
     // InArray 문자열 조각들로 채워질 배열
     // pchDelim 구분자로 사용할 문자열
     // InCullEmpty 1일 경우, 빈 문자열은 배열에 추가되지 않습니다.
     //return InArray에 있는 요소의 개수  
    this->PartialMessage.ParseIntoArray(Lines, TEXT("\r\n"), false);

    // 마지막 줄이 완전하지 않을 수 있음, 해당 점미사로 끝나는지 확인하는 함수.
    if (this->PartialMessage.EndsWith(TEXT("\r\n")) == false)
    {
        if (Lines.Num() > 0)
        {
            // 마지막 줄은 다음에 받을 메시지와 합치기 위해 보관
            this->PartialMessage = Lines.Last();
            Lines.RemoveAt(Lines.Num() - 1);
        }
    }
    else
    {
        // 모든 줄이 완전함
        this->PartialMessage.Empty();
    }

    // 완전한 메시지들 처리
    for (const FString& Line : Lines)
    {
        if (!Line.IsEmpty())
        {
            UE_LOG(LogChatManager, Log, TEXT("메시지 수신: %s"), *Line);

            // 메시지 받았다고 알림(등록해야함).
            OnMessageReceived.Broadcast(Line);
        }
    }
}

FString UChatManager::BytesToString(const uint8* Data, int32 Length)
{
    // 받은 문자열이 온전하지 못하면.
    if (Data == nullptr || Length <= 0)
    {
        // 빈문자열 리턴.
        return (FString());
    }

    // UTF-8 바이트 배열을 안전하게 null-terminate 처리
    TArray<uint8> SafeData;
    SafeData.Append(Data, Length);
    SafeData.Add(0);  // null terminator 추가

    // UTF-8을 FString으로 변환
    FString Result = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(SafeData.GetData())));

    return (Result);
}

TArray<uint8> UChatManager::StringToBytes(const FString& String)
{
    TArray<uint8> Result;

    // 비어있으면
    if (String.IsEmpty())
    {
        return (Result);
    }

    // FString을 UTF-8 바이트로 변환
    FTCHARToUTF8 UTF8String(*String);

    // 바이트 배열에 복사 - UTF-8의 인코딩 방식의 바이트로 변경해서 저장.
    Result.Append(reinterpret_cast<const uint8*>(UTF8String.Get()), UTF8String.Length());

    return (Result);
}