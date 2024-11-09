// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))


{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();

	}

}


//��������
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumpublicConnections, FString MatchType)   
{
	//�жϽӿ��Ƿ���Ч
	if (!SessionInterface.IsValid())
	{
		return;
	}


	// ����Ƿ���ǰ���ڻỰ
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession != nullptr)   	// �����ǰ���ڻỰ
	{
		// ���ٻỰ
		bCreateSessionOnDestroy = true;						// ���ε��� CreateSession() ��Ҫ������ǰ�Ự
		LastNumPublicConnections = NumpublicConnections;	// �����ϴλỰ����������
		LastMatchType = MatchType;							// �����ϴλỰƥ������

		DestroySession();	// ���ٻỰ
		return;
	
	}

	// ����ί�о�����Ա�˺��Ƴ�ί���б�
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);	// ���ί�е��Ự�ӿڵ�ί���б�

	// FOnlineSessionSettings ��ͷ�ļ� "OnlineSessionSettings.h" ��
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());	// �����Ự���ã����ú��� MakeShareable ��ʼ��

	// �Ự���ó�Ա�������ļ����壺
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;	// �Ự���ã�����ҵ�����ϵͳ����Ϊ ��NULL������ʹ�� LAN ���ӣ�����ʹ��
	LastSessionSettings->NumPublicConnections = NumpublicConnections;	// �Ự���ã�������󹫹�������Ϊ����������� NumpublicConnections
	LastSessionSettings->bAllowJoinInProgress = true;					// �Ự���ã��ڻỰ����ʱ����������Ҽ���
	LastSessionSettings->bAllowJoinViaPresence = true;					// �Ự���ã�Steam ʹ�� Presence �����Ự���ڵ�����ȷ��������������
	LastSessionSettings->bShouldAdvertise = true;						// �Ự���ã����� Steam �����Ự
	LastSessionSettings->bUsesPresence = true;							// �Ự���ã�������ʾ�û� Presence ��Ϣ
	LastSessionSettings->bUseLobbiesIfAvailable = true;					// �Ự���ã�����ѡ�� Lobby API��Steam ֧�� Lobby API��
	LastSessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);	
	LastSessionSettings->BuildUniqueId = 1;
	// �Ự���ã�ƥ������



	// �����Ự
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();	// ��ȡ�������ָ��
	/*
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),			// ��һ����������Ϊ strut FUniqueNetIdRepl�������̳��� struct FUniqueNetIdWrapper
																						// �����װ����������������� *������ʾ * ����һ������ *UniquenetId
									NAME_GameSession,									// �ڶ�����������Ϊ FName SessionName����Ϸ�Ự����
									*LastSessionSettings);								// ��������������Ϊ const FOnlineSessionSettings &NewSessionSettings
	*/
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings)) 
	{
		// ����Ự����ʧ�ܣ���ί���Ƴ�ί���б�
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		//���������Լ����Զ���ί��
		// �㲥�Ự����ʧ����Ϣ���Զ������ϵͳί��
		SubsystemOnCreateSessionCompleteDelegate.Broadcast(false);


	}

}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);


		LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
		LastSessionSearch->MaxSearchResults = MaxSearchResults;
		LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);	
		// �Ự�������ã���ѯ���ã�ȷ���κβ��ҵ��ĻỰ��ʹ���� Presence
	

		//��ȡ�������ָ��
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef())) {
			// ��������Ựʧ�ܣ���ί���Ƴ�ί���б�
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

			// �㲥�����Ự����������ʧ����Ϣ���Զ������ϵͳί��
			SubsystemOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		}
	}
	

}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{

	/* P22 �Ӳ˵��м���Ự��Join Sessions from The Menu��*/
	// ���Ự�ӿ��Ƿ���Ч
	if (!SessionInterface.IsValid()) {	// ����Ự�ӿ���Ч
		SubsystemOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);	// �㲥����ʧ����Ϣ���Զ������ϵͳί��
		return;
	}

	// ����ί�о�����Ա�˺��Ƴ�ί���б�
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// ����Ự
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();	//��ȡ�������ָ��
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult)) {
		// �������Ựʧ�ܣ���ί���Ƴ�ί���б�
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		// �㲥����Ựʧ����Ϣ���Զ������ϵͳί��
		SubsystemOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}

}

void UMultiplayerSessionsSubsystem::DestroySession()
{

	// ���Ự�ӿ��Ƿ���Ч
	if (!SessionInterface.IsValid()) {	// ����Ự�ӿ���Ч
		SubsystemOnDestroySessionCompleteDelegate.Broadcast(false);	// �㲥�Ự����ʧ����Ϣ���Զ������ϵͳί��
		return;
	}

	// ����ί�о�����Ա�˺��Ƴ�ί���б�
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);	// ���ί�е��Ự�ӿڵ�ί���б�

	// ���ٻỰ
	if (!SessionInterface->DestroySession(NAME_GameSession)) {
		// ������ٻỰʧ�ܣ���ί���Ƴ�ί���б�
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

		// �㲥�Ự����ʧ����Ϣ���Զ������ϵͳί��
		SubsystemOnDestroySessionCompleteDelegate.Broadcast(false);
	}


}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// �Ự�����ɹ�����ί���Ƴ�ί���б�
	if (SessionInterface) 
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// �㲥�Ự�����ɹ���Ϣ���Զ������ϵͳί��
	SubsystemOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);


}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{

	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		SubsystemOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(),false);

		return;

	}
	SubsystemOnFindSessionsCompleteDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);

}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	/* P22 �Ӳ˵��м���Ự��Join Sessions from The Menu��*/
	// �������Ự�ɹ�����ί���Ƴ�ί���б�
	if (SessionInterface) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// �㲥����Ự�Ľ�����Զ������ϵͳί��
	SubsystemOnJoinSessionCompleteDelegate.Broadcast(Result);
	/* P22 �Ӳ˵��м���Ự��Join Sessions from The Menu��*/




}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// ������ٻỰ�ɹ�����ί���Ƴ�ί���б�
	if (SessionInterface) {
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// ����ϴε��� CreateSession() ʱ��ǰ�ĻỰ��Ҫ�������ҸûỰ�Ѿ����ٳɹ�
	if (bCreateSessionOnDestroy && bWasSuccessful) {
		bCreateSessionOnDestroy = false;						// �ָ���ʼֵ
		CreateSession(LastNumPublicConnections, LastMatchType);	// �����»Ự
	}

	// �㲥���ٻỰ�Ľ�����Զ������ϵͳί��
	SubsystemOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);



}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{




}
