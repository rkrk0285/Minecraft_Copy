// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h" // Enhanced Input 시스템을 사용하기 위한 헤더
#include "EnhancedInputSubsystems.h" // Input Mapping Context를 추가하기 위한 헤더
#include "InputActionValue.h" // FInputActionValue를 사용하기 위한 헤더
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h" // 캐릭터 이동 기능을 사용하기 위한 헤더
#include "GameFramework/Controller.h" // 컨트롤러에서 방향 벡터를 가져오기 위한 헤더
#include "Components/InputComponent.h" // 기본 입력 처리용 헤더
#include "Camera/CameraComponent.h" // 카메라 컴포넌트 사용을 위한 헤더
#include "GameFramework/SpringArmComponent.h" // Spring Arm (카메라 붐) 사용을 위한 헤더

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Spring Arm (카메라 붐) 생성 및 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 캐릭터와 카메라 거리 설정
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 기준으로 회전

	// 카메라 생성 및 Spring Arm에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 자체 회전 X

	// 캐릭터 회전 설정

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (MappingContext.Object) 
	{
		DefaultMappingContext = MappingContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MOVEACTION(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'"));
	if (MOVEACTION.Object) 
	{
		MoveAction = MOVEACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LOOKACTION(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Look.IA_Look'"));
	if (LOOKACTION.Object) 
	{
		LookAction = LOOKACTION.Object;
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// 매핑 컨텍스트를 할당하는 역할.
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		//Subsystem->RemoveMappingContext(DefaultMappingContext);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 캐스팅 전 체크, 없는 경우 완벽한 Null 을 보장
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		//Action에 따른 함수 바인드
		//Moving		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveValue = Value.Get<FVector2D>();
	if (Controller)
	{		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);
	}
}


void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Log, TEXT("%s"), *LookValue.ToString());
	AddControllerYawInput(LookValue.X);   // 좌우 회전
	AddControllerPitchInput(LookValue.Y); // 상하 회전
}

