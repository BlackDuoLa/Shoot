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

	// 检查会话接口是否有效
	if (!OnlineSessionInterface.IsValid()) {
		return;
	}

	// 检查是否先前存在会话
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {								// 如果先前存在会话
		OnlineSessionInterface->DestroySession(NAME_GameSession);	// 销毁会话
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);	// 添加委托到会话接口的委托列表

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());	// 创建会话设置，利用函数 MakeShareable 初始化
	// FOnlineSessionSettings 在头文件 "OnlineSessionSettings.h" 中

// 会话设置成员变量参阅及含义：
	SessionSettings->bIsLANMatch = false;			// 会话设置：不创建 LAN 连接
	SessionSettings->NumPublicConnections = 4;		// 会话设置：设置最大公共连接数为 4
	SessionSettings->bAllowJoinInProgress = true;	// 会话设置：在会话运行时允许其他玩家加入
	SessionSettings->bAllowJoinViaPresence = true;	// 会话设置：Steam 使用 Presence 搜索会话所在地区，确保连接正常工作
	SessionSettings->bShouldAdvertise = true;		// 会话设置：允许 Steam 发布会话
	SessionSettings->bUsesPresence = true;			// 会话设置：允许显示用户 Presence 信息
	SessionSettings->bUseLobbiesIfAvailable = true;	// （视频中未提及）会话设置：优先选择 Lobby API（Steam 支持 Lobby API）
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();					// 获取本地玩家指针
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),	// 第一个参数类型为 strut FUniqueNetIdRepl，公共继承了 struct FUniqueNetIdWrapper
		// 这个包装器重载了引用运算符 *，它表示 * 返回一个引用 *UniquenetId
		NAME_GameSession,							// 第二个参数类型为 FName SessionName，游戏会话名称
		*SessionSettings);


}

void AShootCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("TextCreate")));

	if (bWasSuccessful)
	{
		//创建成功打印成功
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Created Session: %s"),*SessionName.ToString()));
		}
	}

	else
	{
		//创建失败打印失败
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




