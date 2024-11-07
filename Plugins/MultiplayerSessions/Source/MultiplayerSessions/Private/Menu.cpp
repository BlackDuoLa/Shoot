

#include "Menu.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch )
{
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
		if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
				-1,				// 使用 -1 不会覆盖前面的调试信息
				15.f,			// 调试信息的显示时间
				FColor::Yellow,	// 字体颜色：黄色
				FString::Printf(TEXT("Session created Successfully!"))	// 打印会话创建成功消息
			);
		}

		// 会话创建成功后传送至关卡 Lobby
		UWorld* World = GetWorld();
		if (World)
		{
	
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));	// 作为监听服务器打开 Lobby 关卡
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
				-1,				// 使用 -1 不会覆盖前面的调试信息
				15.f,			// 调试信息的显示时间
				FColor::Yellow,	// 字体颜色：黄色
				FString::Printf(TEXT("Failed to create session!"))	// 打印会话创建成功消息
			);
		}
	}


}


void UMenu::HostButtonClicked()
{

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);	// 创建游戏会话


		//if (GEngine) {
		//	GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
		//		-1,				// 使用 -1 不会覆盖前面的调试信息
		//		15.f,			// 调试信息的显示时间
		//		FColor::Yellow,	// 字体颜色：黄色
		//		FString::Printf(TEXT("Host Button Clicked"))	// 打印会话创建成功消息
		//	);
		//}


	}



}

void UMenu::JoinButtonClicked()
{

	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
			-1,				// 使用 -1 不会覆盖前面的调试信息
			15.f,			// 调试信息的显示时间
			FColor::Yellow,	// 字体颜色：黄色
			FString::Printf(TEXT("Join Button Clicked1!"))	// 打印点击事件消息
		);
	}

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

