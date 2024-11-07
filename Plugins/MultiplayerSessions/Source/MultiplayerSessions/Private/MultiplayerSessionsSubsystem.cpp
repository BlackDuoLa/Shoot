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
	//判断接口是否有效
	if (!SessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);

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
