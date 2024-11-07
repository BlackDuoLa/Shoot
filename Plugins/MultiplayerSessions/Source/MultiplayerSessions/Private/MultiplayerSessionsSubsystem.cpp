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

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumpublicConnections, FString MatchType)
{
	//�жϽӿ��Ƿ���Ч
	if (!SessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);

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
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
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
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
