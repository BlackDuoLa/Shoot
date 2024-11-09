

#include "Menu.h"
#include "Kismet/KismetSystemLibrary.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch , FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"),*LobbyPath);

	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	/* P19��ʵ����ϵͳ�����������Ự��Create Session��*/


	AddToViewport();							// ��ӵ��ӿ�
	SetVisibility(ESlateVisibility::Visible);	// ���ò˵��ɼ�
	bIsFocusable = true; 						// �����������ʱ��۽�



	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();	// ��ȡ��ҿ�����ָ��
		if (PlayerController)
		{
			FInputModeUIOnly InputModeSettings;	// ��������ֻ������� UI ������ģʽ

			// ����ģʽ����
			InputModeSettings.SetWidgetToFocus(TakeWidget());	// ���ý��࣬TakeWidget()����ȡ�ײ�� slate ���������������������
			InputModeSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	//����������ӿ�ʱ������

			// �����úõ�����ģʽ���͵�����ҿ�����������ģʽ
			PlayerController->SetInputMode(InputModeSettings);	// ������ҿ�����������ģʽ
			PlayerController->SetShowMouseCursor(true);			// ��ʾ�����
		}
	}


	/* P18 �������ǵ���ϵͳ��Acessing Our Subsystem��*/
	UGameInstance* GameInstance = GetGameInstance();	// ��ȡ��Ϸʵ��
	if (GameInstance) 
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();	// ������ϵͳ
	}
	/* P18 �������ǵ���ϵͳ��Acessing Our Subsystem��*/


	// �󶨲˵���ص���������ϵͳί����
	if (MultiplayerSessionsSubsystem) 
	{
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
	}


	// �󶨲˵���ص���������ϵͳί����
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);

		/* P21 �������ϵͳί�У�More Subsystem Delegates��*/
		MultiplayerSessionsSubsystem->SubsystemOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->SubsystemOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->SubsystemOnDestroySessionCompleteDelegate.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->SubsystemOnStartSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnStartSession);
		/* P21 �������ϵͳί�У�More Subsystem Delegates��*/
	}

}

bool UMenu::Initialize()
{
	// ���ø���� Initialize() ����
	if (!Super::Initialize()) {	// ����ʼ��ʧ�ܣ�ֱ���˳�
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);	// �󶨻ص����� HostButtonClicked()
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);	// �󶨻ص����� JoinButtonClicked()
	}
	if (QuiteGameButton)
	{
		// �󶨰�ť����¼����˳�����
		QuiteGameButton->OnClicked.AddDynamic(this, &ThisClass::QuiteGameButtonClicked);
	}

	return true;

}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();	// ���ø���� NativeDestruct() ����

}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// �Ự�����ɹ��������ؿ� Lobby
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);	// ��Ϊ������������ Lobby �ؿ�
		}

		else 
		{

			// ����Ự����ʧ�ܣ����������� HostButton ��ť
			/*HostButton->SetIsEnabled(true);*/

		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{

	if (MultiplayerSessionsSubsystem == nullptr) {
		return;
	}

	// ������������������һ��ƥ��������ͬ�ĻỰ���Ժ���Խ��иĽ���
	for (auto Result : SearchResults) {
		FString SettingsValue;	// ����Ựƥ������
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);	// ��ȡ�Ựƥ������
		if (SettingsValue == MatchType) {						// ���ƥ��������ͬ
			MultiplayerSessionsSubsystem->JoinSession(Result);	// ������ϵͳ�ļ���Ự����
			return;
		}
	}
	// ��������Ựʧ�ܻ����������Ϊ 0
	//if (!bWasSuccessful || SearchResults.Num() == 0)
	//{
	//	// �������� JoinButton ��ť
	//	JoinButton->SetIsEnabled(true);
	//}

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	/* P22 �Ӳ˵��м���Ự��Join Sessions from The Menu��*/
	// ����Ự�����������ؿ� ��Lobby��
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();						// ��ȡ��ǰ��������ϵͳָ��
	if (OnlineSubsystem) {																// �����ǰ������ϵͳ��Ч
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();	// ��ȡ�Ự�ӿ�����ָ��
		if (SessionInterface.IsValid()) {	// �����ȡ�Ự�ӿڳɹ�
			FString Address;				// ����Ự����Դ��ַ
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);		// ��ȡ�Ự����Դ��ַ

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();	// ��ȡ��ҿ�����
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);	// �ͻ��˴������ؿ� ��Lobby��
			}
		}
	}


	
	// ��������Ự�ɹ�������Ựʧ��
	//if (Result != EOnJoinSessionCompleteResult::Success)
	//{
	//	// �������� JoinButton ��ť
	//	JoinButton->SetIsEnabled(true);
	//}

}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}


void UMenu::HostButtonClicked()
{
	//���ð�ť
	//HostButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);	// ������Ϸ�Ự

	}

}

//�������
void UMenu::JoinButtonClicked()
{

	//���ð�ť
	//JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
	


}

void UMenu::QuiteGameButtonClicked()
{

	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);

}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) 
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();	// ��ȡ��ҿ�����ָ��
		if (PlayerController) {
			FInputModeGameOnly InputModeData;				// �������ÿ��Կ�����Ϸ������ģʽ
			PlayerController->SetInputMode(InputModeData);	// ������ҿ�����������ģʽ
			PlayerController->SetShowMouseCursor(false);	// ���������
		}
	}
}

