
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


	//菜单类
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")),FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby?listen")));


protected:

	virtual bool Initialize() override;	// 初始化函数重写，绑定按钮与回调函数

	//切换地图函数
	virtual void NativeDestruct() override;


	//创建会话
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	//发现会话
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	//加入会话
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	// 销毁会话
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	//开始会话
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	



private:

	//绑定按钮HostButton
	UPROPERTY(meta = (BindWidget))		// 与虚幻引擎中的按钮控件链接
	class UButton* HostButton;			// 保证 C++ 变量名和虚幻引擎中的按钮控件名称相同

	//绑定按钮JoinButton
	UPROPERTY(meta = (BindWidget))		// 与虚幻引擎中的按钮控件链接
	UButton* JoinButton;				// 保证 C++ 变量名和虚幻引擎中的按钮控件名称相同

	UPROPERTY(meta = (BindWidget))
	UButton* QuiteGameButton;


	UFUNCTION()
	void HostButtonClicked();			// 回调函数：响应鼠标单击 HostButton 事件
	UFUNCTION()
	void JoinButtonClicked();			// 回调函数：响应鼠标单击 JoinButton 事件

	UFUNCTION()
	void QuiteGameButtonClicked();


	void MenuTearDown();				// 撤销先前设置的输入模式

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;	// 处理所有在线会话功能的子系统

	
	int32 NumPublicConnections{ 4 };			// 公共连接数
	FString MatchType = { TEXT("FreeForAll") };	// 匹配类型
	
	FString PathToLobby{ TEXT("") };


};
