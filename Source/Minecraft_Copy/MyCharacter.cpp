// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h" // Enhanced Input �ý����� ����ϱ� ���� ���
#include "EnhancedInputSubsystems.h" // Input Mapping Context�� �߰��ϱ� ���� ���
#include "InputActionValue.h" // FInputActionValue�� ����ϱ� ���� ���
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h" // ĳ���� �̵� ����� ����ϱ� ���� ���
#include "GameFramework/Controller.h" // ��Ʈ�ѷ����� ���� ���͸� �������� ���� ���
#include "Components/InputComponent.h" // �⺻ �Է� ó���� ���
#include "Camera/CameraComponent.h" // ī�޶� ������Ʈ ����� ���� ���
#include "GameFramework/SpringArmComponent.h" // Spring Arm (ī�޶� ��) ����� ���� ���

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Spring Arm (ī�޶� ��) ���� �� ����
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // ĳ���Ϳ� ī�޶� �Ÿ� ����
	CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ� �������� ȸ��

	// ī�޶� ���� �� Spring Arm�� ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false; // ī�޶�� ��ü ȸ�� X

	// ĳ���� ȸ�� ����

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
		// ���� ���ؽ�Ʈ�� �Ҵ��ϴ� ����.
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

	// ĳ���� �� üũ, ���� ��� �Ϻ��� Null �� ����
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		//Action�� ���� �Լ� ���ε�
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
	AddControllerYawInput(LookValue.X);   // �¿� ȸ��
	AddControllerPitchInput(LookValue.Y); // ���� ȸ��
}

