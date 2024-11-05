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


	//�˵���
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));


protected:

	virtual bool Initialize() override;	// ��ʼ��������д���󶨰�ť��ص�����
	virtual void NativeDestruct() override;


	/* P20 ������ϵͳ�Ļص�������Callbacks to Our Subsystem��*/
	void OnCreateSession(bool bWasSuccessful);





private:


	UPROPERTY(meta = (BindWidget))		// ����������еİ�ť�ؼ�����
	class UButton* HostButton;			// ��֤ C++ ����������������еİ�ť�ؼ�������ͬ


	UPROPERTY(meta = (BindWidget))		// ����������еİ�ť�ؼ�����
	UButton* JoinButton;				// ��֤ C++ ����������������еİ�ť�ؼ�������ͬ


	UFUNCTION()
	void HostButtonClicked();			// �ص���������Ӧ��굥�� HostButton �¼�

	UFUNCTION()
	void JoinButtonClicked();			// �ص���������Ӧ��굥�� JoinButton �¼�


	void MenuTearDown();				// ������ǰ���õ�����ģʽ


	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;	// �����������߻Ự���ܵ���ϵͳ

	int32 NumPublicConnections;
	FString MatchType{ TEXT("FreeForAll") };

};
