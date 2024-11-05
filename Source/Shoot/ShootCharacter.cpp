// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"



//////////////////////////////////////////////////////////////////////////
// AShootCharacter

AShootCharacter::AShootCharacter():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem % s"), *OnlineSubsystem->GetSubsystemName().ToString()));

			
		}
	}
}

void AShootCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShootCharacter::CreateGameSession()
{

	// ���Ự�ӿ��Ƿ���Ч
	if (!OnlineSessionInterface.IsValid()) {
		return;
	}

	// ����Ƿ���ǰ���ڻỰ
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {								// �����ǰ���ڻỰ
		OnlineSessionInterface->DestroySession(NAME_GameSession);	// ���ٻỰ
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);	// ���ί�е��Ự�ӿڵ�ί���б�

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());	// �����Ự���ã����ú��� MakeShareable ��ʼ��
	// FOnlineSessionSettings ��ͷ�ļ� "OnlineSessionSettings.h" ��

// �Ự���ó�Ա�������ļ����壺
	SessionSettings->bIsLANMatch = false;			// �Ự���ã������� LAN ����
	SessionSettings->NumPublicConnections = 4;		// �Ự���ã�������󹫹�������Ϊ 4
	SessionSettings->bAllowJoinInProgress = true;	// �Ự���ã��ڻỰ����ʱ����������Ҽ���
	SessionSettings->bAllowJoinViaPresence = true;	// �Ự���ã�Steam ʹ�� Presence �����Ự���ڵ�����ȷ��������������
	SessionSettings->bShouldAdvertise = true;		// �Ự���ã����� Steam �����Ự
	SessionSettings->bUsesPresence = true;			// �Ự���ã�������ʾ�û� Presence ��Ϣ
	SessionSettings->bUseLobbiesIfAvailable = true;	// ����Ƶ��δ�ἰ���Ự���ã�����ѡ�� Lobby API��Steam ֧�� Lobby API��
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();					// ��ȡ�������ָ��
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),	// ��һ����������Ϊ strut FUniqueNetIdRepl�������̳��� struct FUniqueNetIdWrapper
		// �����װ����������������� *������ʾ * ����һ������ *UniquenetId
		NAME_GameSession,							// �ڶ�����������Ϊ FName SessionName����Ϸ�Ự����
		*SessionSettings);


}

void AShootCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("TextCreate")));

	if (bWasSuccessful)
	{
		//�����ɹ���ӡ�ɹ�
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Created Session: %s"),*SessionName.ToString()));
		}
	}

	else
	{
		//����ʧ�ܴ�ӡʧ��
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Failed to create Session")));

		}
	}
}

//,,,,,,,,,,,,,,,,,,,,,,//

//void AShootCharacter::OpenLobby()
//{
//	UWorld* World = GetWorld();
//	if (World)
//	{
//		World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?Listen");
//	}
//
//}
//
//void AShootCharacter::CallOpenLevel(const FString& Address)
//{
//	UGameplayStatics::OpenLevel(this, *Address);
//
//}
//
//void AShootCharacter::CallClientTravel(const FString& Address)
//{
//
//	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
//		if (PlayerController)
//		{
//			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
//
//		}
//}


//...................................................//

//////////////////////////////////////////////////////////////////////////
// Input

void AShootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShootCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShootCharacter::Look);

	}

}

void AShootCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShootCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




