
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:


	//�˵���
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")),FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby?listen")));


protected:

	virtual bool Initialize() override;	// ��ʼ��������д���󶨰�ť��ص�����

	//�л���ͼ����
	virtual void NativeDestruct() override;


	//�����Ự
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	//���ֻỰ
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	//����Ự
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	// ���ٻỰ
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	//��ʼ�Ự
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	



private:

	//�󶨰�ťHostButton
	UPROPERTY(meta = (BindWidget))		// ����������еİ�ť�ؼ�����
	class UButton* HostButton;			// ��֤ C++ ����������������еİ�ť�ؼ�������ͬ

	//�󶨰�ťJoinButton
	UPROPERTY(meta = (BindWidget))		// ����������еİ�ť�ؼ�����
	UButton* JoinButton;				// ��֤ C++ ����������������еİ�ť�ؼ�������ͬ

	UPROPERTY(meta = (BindWidget))
	UButton* QuiteGameButton;


	UFUNCTION()
	void HostButtonClicked();			// �ص���������Ӧ��굥�� HostButton �¼�
	UFUNCTION()
	void JoinButtonClicked();			// �ص���������Ӧ��굥�� JoinButton �¼�

	UFUNCTION()
	void QuiteGameButtonClicked();


	void MenuTearDown();				// ������ǰ���õ�����ģʽ

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;	// �����������߻Ự���ܵ���ϵͳ

	
	int32 NumPublicConnections{ 4 };			// ����������
	FString MatchType = { TEXT("FreeForAll") };	// ƥ������
	
	FString PathToLobby{ TEXT("") };


};
