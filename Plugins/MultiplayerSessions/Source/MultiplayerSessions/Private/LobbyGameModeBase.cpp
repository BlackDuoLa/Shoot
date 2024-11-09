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
			GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
				1,				// Key ���� -1 ʱ�������������Ϣ
				60.f,			// ������Ϣ����ʾʱ��
				FColor::Red,	// ������ɫ����ɫ
				FString::Printf(TEXT("Players in game: %d!"), NumberOfPlayers)	// ��ӡ�������
			);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState) {
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
					2,				// Key ���� -1 ʱ�������������Ϣ
					60.f,			// ������Ϣ����ʾʱ��
					FColor::Cyan,	// ������ɫ������ɫ
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)	// ��ӡ������Ϸ������ǳ�
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
		GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
			1,				// Key ���� -1 ʱ�������������Ϣ
			60.f,			// ������Ϣ����ʾʱ��
			FColor::Red,	// ������ɫ����ɫ
			FString::Printf(TEXT("Players in game: %d!"), NumberOfPlayers - 1)	// ��ӡ���������
			// ��ʱ PlayerArray.Num() ��δ���£�������м� 1 ����ֻ��Ϊ�˷������ʱ��ʾ��ȷ����������ʵ����Ŀ�в�����˲���
		);


		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
			2,				// Key ���� -1 ʱ�������������Ϣ
			60.f,			// ������Ϣ����ʾʱ��
			FColor::Cyan,	// ������ɫ������ɫ
			FString::Printf(TEXT("%s has exited the game!"), *PlayerName)	// ��ӡ������Ϸ������ǳ�
		);
	}





}
