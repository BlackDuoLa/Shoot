// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));


protected:

	virtual bool Initialize() override;	// 初始化函数重写，绑定按钮与回调函数
	virtual void NativeDestruct() override;


	/* P20 我们子系统的回调函数（Callbacks to Our Subsystem）*/
	void OnCreateSession(bool bWasSuccessful);





private:


	UPROPERTY(meta = (BindWidget))		// 与虚幻引擎中的按钮控件链接
	class UButton* HostButton;			// 保证 C++ 变量名和虚幻引擎中的按钮控件名称相同


	UPROPERTY(meta = (BindWidget))		// 与虚幻引擎中的按钮控件链接
	UButton* JoinButton;				// 保证 C++ 变量名和虚幻引擎中的按钮控件名称相同


	UFUNCTION()
	void HostButtonClicked();			// 回调函数：响应鼠标单击 HostButton 事件

	UFUNCTION()
	void JoinButtonClicked();			// 回调函数：响应鼠标单击 JoinButton 事件


	void MenuTearDown();				// 撤销先前设置的输入模式


	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;	// 处理所有在线会话功能的子系统

	int32 NumPublicConnections;
	FString MatchType{ TEXT("FreeForAll") };

};
