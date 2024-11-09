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

/* P21 更多的子系统委托（More Subsystem Delegates）*/
DECLARE_MULTICAST_DELEGATE_TwoParams(FSubsystemOnFindSessionsCompleteDelegate, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
// FOnlineSessionSearchResult 不是 UCLASS 或 USTRUCT，不能在蓝图中实现回调，因此这里使用静态多播委托
// 如果想要实现整个菜单类和蓝图，并从蓝图中进行回调，需要使用事件调度器，
// 创建一个 UCLASS 或 USTRUCT，其中包含有关搜索结果的代码，然后使用动态多播委托传递信息
// 但我们正在使用 C++ 而非蓝图实现菜单类，为了让工作不会过于复杂，我们只使用静态多播委托而非动态。

DECLARE_MULTICAST_DELEGATE_OneParam(FSubsystemOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);
// EOnJoinSessionCompleteResult 也是不是 UCLASS 或 USTRUCT，不能在蓝图中实现回调，因此这里也要使用静态多播委托

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnDestroySessionCompleteDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubsystemOnStartSessionCompleteDelegate, bool, bWasSuccessful);
/* P21 更多的子系统委托（More Subsystem Delegates）*/





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



	/* P21 更多的子系统委托（More Subsystem Delegates）*/
	FSubsystemOnFindSessionsCompleteDelegate SubsystemOnFindSessionsCompleteDelegate;
	FSubsystemOnJoinSessionCompleteDelegate SubsystemOnJoinSessionCompleteDelegate;
	FSubsystemOnDestroySessionCompleteDelegate SubsystemOnDestroySessionCompleteDelegate;
	FSubsystemOnStartSessionCompleteDelegate SubsystemOnStartSessionCompleteDelegate;
	/* P21 更多的子系统委托（More Subsystem Delegates）*/


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

	TSharedPtr< FOnlineSessionSearch>LastSessionSearch;

	
	
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


	bool bCreateSessionOnDestroy{ false };		// 上次调用 CreateSession() 时先前会话是否存在且需要被销毁
	int32 LastNumPublicConnections;				// 上次会话的公共连接数
	FString LastMatchType;						// 上次会话的匹配类型
	

};
