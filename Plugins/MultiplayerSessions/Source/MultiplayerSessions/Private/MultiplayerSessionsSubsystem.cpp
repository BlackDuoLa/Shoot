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


//创建房间
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumpublicConnections, FString MatchType)   
{
	//判断接口是否有效
	if (!SessionInterface.IsValid())
	{
		return;
	}


	// 检查是否先前存在会话
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession != nullptr)   	// 如果先前存在会话
	{
		// 销毁会话
		bCreateSessionOnDestroy = true;						// 本次调用 CreateSession() 需要销毁先前会话
		LastNumPublicConnections = NumpublicConnections;	// 保存上次会话公共连接数
		LastMatchType = MatchType;							// 保存上次会话匹配类型

		DestroySession();	// 销毁会话
		return;
	
	}

	// 保存委托句柄，以便此后移出委托列表
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);	// 添加委托到会话接口的委托列表

	// FOnlineSessionSettings 在头文件 "OnlineSessionSettings.h" 中
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());	// 创建会话设置，利用函数 MakeShareable 初始化

	// 会话设置成员变量参阅及含义：
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;	// 会话设置：如果找到的子系统名称为 “NULL”，则使用 LAN 连接，否则不使用
	LastSessionSettings->NumPublicConnections = NumpublicConnections;	// 会话设置：设置最大公共连接数为函数输入变量 NumpublicConnections
	LastSessionSettings->bAllowJoinInProgress = true;					// 会话设置：在会话运行时允许其他玩家加入
	LastSessionSettings->bAllowJoinViaPresence = true;					// 会话设置：Steam 使用 Presence 搜索会话所在地区，确保连接正常工作
	LastSessionSettings->bShouldAdvertise = true;						// 会话设置：允许 Steam 发布会话
	LastSessionSettings->bUsesPresence = true;							// 会话设置：允许显示用户 Presence 信息
	LastSessionSettings->bUseLobbiesIfAvailable = true;					// 会话设置：优先选择 Lobby API（Steam 支持 Lobby API）
	LastSessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);	
	LastSessionSettings->BuildUniqueId = 1;
	// 会话设置：匹配类型



	// 创建会话
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();	// 获取本地玩家指针
	/*
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),			// 第一个参数类型为 strut FUniqueNetIdRepl，公共继承了 struct FUniqueNetIdWrapper
																						// 这个包装器重载了引用运算符 *，它表示 * 返回一个引用 *UniquenetId
									NAME_GameSession,									// 第二个参数类型为 FName SessionName，游戏会话名称
									*LastSessionSettings);								// 第三个参数类型为 const FOnlineSessionSettings &NewSessionSettings
	*/
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings)) 
	{
		// 如果会话创建失败，将委托移出委托列表
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		//播放我们自己的自定义委托
		// 广播会话创建失败消息到自定义的子系统委托
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
		// 会话搜索设置：查询设置，确保任何查找到的会话都使用了 Presence
	

		//获取本地玩家指针
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef())) {
			// 如果搜索会话失败，将委托移出委托列表
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

			// 广播搜索会话结果空数组和失败消息到自定义的子系统委托
			SubsystemOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		}
	}
	

}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{

	/* P22 从菜单中加入会话（Join Sessions from The Menu）*/
	// 检查会话接口是否有效
	if (!SessionInterface.IsValid()) {	// 如果会话接口无效
		SubsystemOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);	// 广播加入失败消息到自定义的子系统委托
		return;
	}

	// 保存委托句柄，以便此后移出委托列表
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// 加入会话
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();	//获取本地玩家指针
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult)) {
		// 如果加入会话失败，将委托移出委托列表
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		// 广播加入会话失败消息到自定义的子系统委托
		SubsystemOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}

}

void UMultiplayerSessionsSubsystem::DestroySession()
{

	// 检查会话接口是否有效
	if (!SessionInterface.IsValid()) {	// 如果会话接口无效
		SubsystemOnDestroySessionCompleteDelegate.Broadcast(false);	// 广播会话销毁失败消息到自定义的子系统委托
		return;
	}

	// 保存委托句柄，以便此后移出委托列表
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);	// 添加委托到会话接口的委托列表

	// 销毁会话
	if (!SessionInterface->DestroySession(NAME_GameSession)) {
		// 如果销毁会话失败，将委托移出委托列表
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

		// 广播会话销毁失败消息到自定义的子系统委托
		SubsystemOnDestroySessionCompleteDelegate.Broadcast(false);
	}


}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 会话创建成功，将委托移出委托列表
	if (SessionInterface) 
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 广播会话创建成功消息到自定义的子系统委托
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
	/* P22 从菜单中加入会话（Join Sessions from The Menu）*/
	// 如果加入会话成功，将委托移出委托列表
	if (SessionInterface) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 广播加入会话的结果到自定义的子系统委托
	SubsystemOnJoinSessionCompleteDelegate.Broadcast(Result);
	/* P22 从菜单中加入会话（Join Sessions from The Menu）*/




}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 如果销毁会话成功，将委托移出委托列表
	if (SessionInterface) {
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// 如果上次调用 CreateSession() 时先前的会话需要被销毁且该会话已经销毁成功
	if (bCreateSessionOnDestroy && bWasSuccessful) {
		bCreateSessionOnDestroy = false;						// 恢复初始值
		CreateSession(LastNumPublicConnections, LastMatchType);	// 创建新会话
	}

	// 广播销毁会话的结果到自定义的子系统委托
	SubsystemOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);



}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{




}
