

#include "Menu.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch )
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	/* P19��ʵ����ϵͳ�����������Ự��Create Session��*/


	AddToViewport();							// ��ӵ��ӿ�
	SetVisibility(ESlateVisibility::Visible);	// ���ò˵��ɼ�
	bIsFocusable = true; 						// �����������ʱ��۽�



	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();	// ��ȡ��ҿ�����ָ��
		if (PlayerController)
		{
			FInputModeUIOnly InputModeSettings;	// ��������ֻ������� UI ������ģʽ

			// ����ģʽ����
			InputModeSettings.SetWidgetToFocus(TakeWidget());	// ���ý��࣬TakeWidget()����ȡ�ײ�� slate ���������������������
			InputModeSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	//����������ӿ�ʱ������

			// �����úõ�����ģʽ���͵�����ҿ�����������ģʽ
			PlayerController->SetInputMode(InputModeSettings);	// ������ҿ�����������ģʽ
			PlayerController->SetShowMouseCursor(true);			// ��ʾ�����
		}
	}


	/* P18 �������ǵ���ϵͳ��Acessing Our Subsystem��*/
	UGameInstance* GameInstance = GetGameInstance();	// ��ȡ��Ϸʵ��
	if (GameInstance) 
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();	// ������ϵͳ
	}
	/* P18 �������ǵ���ϵͳ��Acessing Our Subsystem��*/


	// �󶨲˵���ص���������ϵͳί����
	if (MultiplayerSessionsSubsystem) 
	{
		MultiplayerSessionsSubsystem->SubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
	}

}

bool UMenu::Initialize()
{
	// ���ø���� Initialize() ����
	if (!Super::Initialize()) {	// ����ʼ��ʧ�ܣ�ֱ���˳�
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);	// �󶨻ص����� HostButtonClicked()
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);	// �󶨻ص����� JoinButtonClicked()
	}
	return true;

}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();	// ���ø���� NativeDestruct() ����

}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
				-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
				15.f,			// ������Ϣ����ʾʱ��
				FColor::Yellow,	// ������ɫ����ɫ
				FString::Printf(TEXT("Session created Successfully!"))	// ��ӡ�Ự�����ɹ���Ϣ
			);
		}

		// �Ự�����ɹ��������ؿ� Lobby
		UWorld* World = GetWorld();
		if (World)
		{
	
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));	// ��Ϊ������������ Lobby �ؿ�
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
				-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
				15.f,			// ������Ϣ����ʾʱ��
				FColor::Yellow,	// ������ɫ����ɫ
				FString::Printf(TEXT("Failed to create session!"))	// ��ӡ�Ự�����ɹ���Ϣ
			);
		}
	}


}


void UMenu::HostButtonClicked()
{

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);	// ������Ϸ�Ự


		//if (GEngine) {
		//	GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
		//		-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
		//		15.f,			// ������Ϣ����ʾʱ��
		//		FColor::Yellow,	// ������ɫ����ɫ
		//		FString::Printf(TEXT("Host Button Clicked"))	// ��ӡ�Ự�����ɹ���Ϣ
		//	);
		//}


	}



}

void UMenu::JoinButtonClicked()
{

	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(	// ��ӵ�����Ϣ����Ļ��
			-1,				// ʹ�� -1 ���Ḳ��ǰ��ĵ�����Ϣ
			15.f,			// ������Ϣ����ʾʱ��
			FColor::Yellow,	// ������ɫ����ɫ
			FString::Printf(TEXT("Join Button Clicked1!"))	// ��ӡ����¼���Ϣ
		);
	}

}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) 
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();	// ��ȡ��ҿ�����ָ��
		if (PlayerController) {
			FInputModeGameOnly InputModeData;				// �������ÿ��Կ�����Ϸ������ģʽ
			PlayerController->SetInputMode(InputModeData);	// ������ҿ�����������ģʽ
			PlayerController->SetShowMouseCursor(false);	// ���������
		}
	}
}

