

#include "Menu.h"
#include "Kismet/KismetSystemLibrary.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch , FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"),*LobbyPath);

	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	/* P19（实现子系统函数）创建会话（Create Session）*/


	AddToViewport();							// 添加到视口
	SetVisibility(ESlateVisibility::Visible);	// 设置菜单可见
	bIsFocusable = true; 						// 允许鼠标点击的时候聚焦



	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();	// 获取玩家控制器指针
		if (PlayerController)
		{
			FInputModeUIOnly InputModeSettings;	// 用于设置只允许控制 UI 的输入模式

			// 输入模式设置
			InputModeSettings.SetWidgetToFocus(TakeWidget());	// 设置焦距，TakeWidget()将获取底层的 slate 部件，如果不存在则构造它
			InputModeSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	//设置鼠标在视口时不锁定

			// 将设置好的输入模式传送到到玩家控制器的输入模式
			PlayerController->SetInputMode(InputModeSettings);	// 设置玩家控制器的输入模式
			PlayerController->SetShowMouseCursor(true);			// 显示鼠标光标
		}
	}


	/* P18 访问我们的子系统（Acessing Our Subsystem）*/
	UGameInstance* GameInstance = GetGameInstance();	// 获取游戏实例
	if (GameInstance) 
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();	// 访问子系统
	}
	/* P18 访问我们的子系统（Acessing Our Subsystem）*/


	// 绑定菜单类回调函数到子系统委托上
	if (MultiplayerSessionsSubsystem) 
	{
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
	}


	// 绑定菜单类回调函数到子系统委托上
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);

		/* P21 更多的子系统委托（More Subsystem Delegates）*/
		MultiplayerSessionsSubsystem->SubsystemOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->SubsystemOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->SubsystemOnDestroySessionCompleteDelegate.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->SubsystemOnStartSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnStartSession);
		/* P21 更多的子系统委托（More Subsystem Delegates）*/
	}

}

bool UMenu::Initialize()
{
	// 调用父类的 Initialize() 函数
	if (!Super::Initialize()) {	// 若初始化失败，直接退出
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);	// 绑定回调函数 HostButtonClicked()
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);	// 绑定回调函数 JoinButtonClicked()
	}
	if (QuiteGameButton)
	{
		// 绑定按钮点击事件到退出函数
		QuiteGameButton->OnClicked.AddDynamic(this, &ThisClass::QuiteGameButtonClicked);
	}

	return true;

}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();	// 调用父类的 NativeDestruct() 函数

}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 会话创建成功后传送至关卡 Lobby
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);	// 作为监听服务器打开 Lobby 关卡
		}

		else 
		{

			// 如果会话创建失败，则重新启用 HostButton 按钮
			/*HostButton->SetIsEnabled(true);*/

		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{

	if (MultiplayerSessionsSubsystem == nullptr) {
		return;
	}

	// 遍历搜索结果并加入第一个匹配类型相同的会话（以后可以进行改进）
	for (auto Result : SearchResults) {
		FString SettingsValue;	// 保存会话匹配类型
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);	// 获取会话匹配类型
		if (SettingsValue == MatchType) {						// 如果匹配类型相同
			MultiplayerSessionsSubsystem->JoinSession(Result);	// 调用子系统的加入会话函数
			return;
		}
	}
	// 如果搜索会话失败或者搜索结果为 0
	//if (!bWasSuccessful || SearchResults.Num() == 0)
	//{
	//	// 重新启用 JoinButton 按钮
	//	JoinButton->SetIsEnabled(true);
	//}

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	/* P22 从菜单中加入会话（Join Sessions from The Menu）*/
	// 加入会话，并传送至关卡 “Lobby”
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();						// 获取当前的在线子系统指针
	if (OnlineSubsystem) {																// 如果当前在线子系统有效
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();	// 获取会话接口智能指针
		if (SessionInterface.IsValid()) {	// 如果获取会话接口成功
			FString Address;				// 保存会话创建源地址
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);		// 获取会话创建源地址

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();	// 获取玩家控制器
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);	// 客户端传送至关卡 “Lobby”
			}
		}
	}


	
	// 如果搜索会话成功但加入会话失败
	//if (Result != EOnJoinSessionCompleteResult::Success)
	//{
	//	// 重新启用 JoinButton 按钮
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
	//禁用按钮
	//HostButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);	// 创建游戏会话

	}

}

//加入会议
void UMenu::JoinButtonClicked()
{

	//禁用按钮
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
		APlayerController* PlayerController = World->GetFirstPlayerController();	// 获取玩家控制器指针
		if (PlayerController) {
			FInputModeGameOnly InputModeData;				// 用于设置可以控制游戏的输入模式
			PlayerController->SetInputMode(InputModeData);	// 设置玩家控制器的输入模式
			PlayerController->SetShowMouseCursor(false);	// 隐藏鼠标光标
		}
	}
}

