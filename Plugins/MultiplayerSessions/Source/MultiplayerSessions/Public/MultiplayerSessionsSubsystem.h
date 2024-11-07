// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"



// 使用创建动态多播委托的宏来声明将与菜单类上的回调函数绑定的自定义委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnCreateSessionCompleteDelegate, bool, bWasSuccessful);





/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	
public:

	//多人游戏（会话）系统
	UMultiplayerSessionsSubsystem();



	/* P16 会话接口委托（Session Interface Delegates）*/
	// 菜单类将会调用以下会话接口函数
	void CreateSession(int32 NumpublicConnections, FString MatchType);	// 创建游戏会话
	void FindSessions(int32 MaxSearchResults);							// 查找游戏会话
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);	// 加入游戏会话

	void DestroySession();	// 销毁会话
	void StartSession();	// 开始会话



	// 将与菜单类上的回调函数绑定的自定义委托
	FSubsystemOnCreateSessionCompleteDelegate SubsystemOnCreateSessionCompleteDelegate;


protected:

	/* P16 会话接口委托（Session Interface Delegates）*/
// 为将要添加到委托列表的委托定义类内部的回调函数，不需要在类外调用
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);	// 委托 CreateSessionCompleteDelegate 的回调函数
	void OnFindSessionsComplete(bool bWasSuccessful);						// 委托 FindSessionCompleteDelegate 的回调函数
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);	// 委托 JoinSessionCompleteDelegate 的回调函数
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);	// 委托 DestroySessionCompleteDelegate 的回调函数
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);	// 委托 StartSessionCompleteDelegate 的回调函数



private:


	IOnlineSessionPtr SessionInterface;
	/* P19（实现子系统函数）创建会话（Create Session）*/
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;	// 上次创建的会话的设置
	/* P19（实现子系统函数）创建会话（Create Session）*/

	
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;		// 会话创建完成委托
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;		// 会话查找完成委托
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;			// 会话加入完成委托
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;	// 会话销毁完成委托
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;		// 会话开始完成委托



	// 定义委托句柄
	FDelegateHandle CreateSessionCompleteDelegateHandle;				// 会话创建完成委托句柄
	FDelegateHandle FindSessionsCompleteDelegateHandle;					// 会话查找完成委托句柄
	FDelegateHandle JoinSessionCompleteDelegateHandle;					// 会话加入完成委托句柄
	FDelegateHandle DestroySessionCompleteDelegateHandle;				// 会话销毁完成委托句柄
	FDelegateHandle StartSessionCompleteDelegateHandle;					// 会话开始完成委托句柄
	/* P16 会话接口委托（Session Interface Delegates）*/


};
