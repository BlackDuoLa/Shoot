// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();


		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
				1,				// Key 不是 -1 时，则更新现有消息
				60.f,			// 调试信息的显示时间
				FColor::Red,	// 字体颜色：黄色
				FString::Printf(TEXT("Players in game: %d!"), NumberOfPlayers)	// 打印玩家人数
			);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState) {
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
					2,				// Key 不是 -1 时，则更新现有消息
					60.f,			// 调试信息的显示时间
					FColor::Cyan,	// 字体颜色：蓝绿色
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)	// 打印进入游戏的玩家昵称
				);
			}
		}

	}

}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);



	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
			1,				// Key 不是 -1 时，则更新现有消息
			60.f,			// 调试信息的显示时间
			FColor::Red,	// 字体颜色：红色
			FString::Printf(TEXT("Players in game: %d!"), NumberOfPlayers - 1)	// 打印玩家人数，
			// 此时 PlayerArray.Num() 还未更新，这里进行减 1 操作只是为了方便测试时显示正确的人数，在实际项目中不会如此操作
		);


		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(	// 添加调试信息到屏幕上
			2,				// Key 不是 -1 时，则更新现有消息
			60.f,			// 调试信息的显示时间
			FColor::Cyan,	// 字体颜色：蓝绿色
			FString::Printf(TEXT("%s has exited the game!"), *PlayerName)	// 打印进入游戏的玩家昵称
		);
	}





}
