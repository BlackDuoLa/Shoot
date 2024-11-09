// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"



// ʹ�ô�����̬�ಥί�еĺ�����������˵����ϵĻص������󶨵��Զ���ί��
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnCreateSessionCompleteDelegate, bool, bWasSuccessful);

/* P21 �������ϵͳί�У�More Subsystem Delegates��*/
DECLARE_MULTICAST_DELEGATE_TwoParams(FSubsystemOnFindSessionsCompleteDelegate, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
// FOnlineSessionSearchResult ���� UCLASS �� USTRUCT����������ͼ��ʵ�ֻص����������ʹ�þ�̬�ಥί��
// �����Ҫʵ�������˵������ͼ��������ͼ�н��лص�����Ҫʹ���¼���������
// ����һ�� UCLASS �� USTRUCT�����а����й���������Ĵ��룬Ȼ��ʹ�ö�̬�ಥί�д�����Ϣ
// ����������ʹ�� C++ ������ͼʵ�ֲ˵��࣬Ϊ���ù���������ڸ��ӣ�����ֻʹ�þ�̬�ಥί�ж��Ƕ�̬��

DECLARE_MULTICAST_DELEGATE_OneParam(FSubsystemOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);
// EOnJoinSessionCompleteResult Ҳ�ǲ��� UCLASS �� USTRUCT����������ͼ��ʵ�ֻص����������ҲҪʹ�þ�̬�ಥί��

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnDestroySessionCompleteDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnStartSessionCompleteDelegate, bool, bWasSuccessful);
/* P21 �������ϵͳί�У�More Subsystem Delegates��*/





/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	
public:

	//������Ϸ���Ự��ϵͳ
	UMultiplayerSessionsSubsystem();



	/* P16 �Ự�ӿ�ί�У�Session Interface Delegates��*/
	// �˵��ཫ��������»Ự�ӿں���
	void CreateSession(int32 NumpublicConnections, FString MatchType);	// ������Ϸ�Ự
	void FindSessions(int32 MaxSearchResults);							// ������Ϸ�Ự
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);	// ������Ϸ�Ự

	void DestroySession();	// ���ٻỰ
	void StartSession();	// ��ʼ�Ự



	// ����˵����ϵĻص������󶨵��Զ���ί��
	FSubsystemOnCreateSessionCompleteDelegate SubsystemOnCreateSessionCompleteDelegate;



	/* P21 �������ϵͳί�У�More Subsystem Delegates��*/
	FSubsystemOnFindSessionsCompleteDelegate SubsystemOnFindSessionsCompleteDelegate;
	FSubsystemOnJoinSessionCompleteDelegate SubsystemOnJoinSessionCompleteDelegate;
	FSubsystemOnDestroySessionCompleteDelegate SubsystemOnDestroySessionCompleteDelegate;
	FSubsystemOnStartSessionCompleteDelegate SubsystemOnStartSessionCompleteDelegate;
	/* P21 �������ϵͳί�У�More Subsystem Delegates��*/


protected:

	/* P16 �Ự�ӿ�ί�У�Session Interface Delegates��*/
// Ϊ��Ҫ��ӵ�ί���б��ί�ж������ڲ��Ļص�����������Ҫ���������
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);	// ί�� CreateSessionCompleteDelegate �Ļص�����
	void OnFindSessionsComplete(bool bWasSuccessful);						// ί�� FindSessionCompleteDelegate �Ļص�����
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);	// ί�� JoinSessionCompleteDelegate �Ļص�����
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);	// ί�� DestroySessionCompleteDelegate �Ļص�����
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);	// ί�� StartSessionCompleteDelegate �Ļص�����



private:


	IOnlineSessionPtr SessionInterface;
	/* P19��ʵ����ϵͳ�����������Ự��Create Session��*/
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;	// �ϴδ����ĻỰ������
	/* P19��ʵ����ϵͳ�����������Ự��Create Session��*/

	TSharedPtr< FOnlineSessionSearch>LastSessionSearch;

	
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;		// �Ự�������ί��
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;		// �Ự�������ί��
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;			// �Ự�������ί��
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;	// �Ự�������ί��
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;		// �Ự��ʼ���ί��



	// ����ί�о��
	FDelegateHandle CreateSessionCompleteDelegateHandle;				// �Ự�������ί�о��
	FDelegateHandle FindSessionsCompleteDelegateHandle;					// �Ự�������ί�о��
	FDelegateHandle JoinSessionCompleteDelegateHandle;					// �Ự�������ί�о��
	FDelegateHandle DestroySessionCompleteDelegateHandle;				// �Ự�������ί�о��
	FDelegateHandle StartSessionCompleteDelegateHandle;					// �Ự��ʼ���ί�о��
	/* P16 �Ự�ӿ�ί�У�Session Interface Delegates��*/


	bool bCreateSessionOnDestroy{ false };		// �ϴε��� CreateSession() ʱ��ǰ�Ự�Ƿ��������Ҫ������
	int32 LastNumPublicConnections;				// �ϴλỰ�Ĺ���������
	FString LastMatchType;						// �ϴλỰ��ƥ������
	

};
