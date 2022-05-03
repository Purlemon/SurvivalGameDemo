// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetRootComponent());
	CameraSpringArm->TargetArmLength = 350.0f;	// ���۳���
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 45.0f));
	CameraSpringArm->bUsePawnControlRotation = true;	// �������۱�Controller����ת��

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	// -----------------�ƶ�--------------------------------

	bUseControllerRotationYaw = false;	// ȡ��Player��Controller����ת��
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.5f;		  // ��������
	MoveComp->JumpZVelocity = 600.0f;		  // ���������ٶ�
	MoveComp->bOrientRotationToMovement = true;	// �Զ�ת��
	MoveComp->RotationRate = FRotator(0.0f, 800.0f, 0.0f);// ת������

	// ----------------״̬------------------------
	Health = MaxHealth;

}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementStatus();
}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �ƶ�
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainPlayer::MoveRight);

	// �ӽǸ������ת��
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainPlayer::LookUp);

	// ��Ծ
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainPlayer::Jump);

	// ����
	// ǹе��׼
	PlayerInputComponent->BindAction(TEXT("Targeting"), IE_Pressed, this, &AMainPlayer::StartTargeting);
	PlayerInputComponent->BindAction(TEXT("Targeting"), IE_Released, this, &AMainPlayer::EndTargeting);
	PlayerInputComponent->BindAction(TEXT("LongTargeting"), IE_Pressed, this, &AMainPlayer::SwitchTargeting);

}

void AMainPlayer::MoveForward(float Value)
{
	bool bIsFalling = GetCharacterMovement()->IsFalling();
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsFalling)
	{
		// �õ�Controller����ת��
		FRotator ControllerRotation = Controller->GetControlRotation();
		FRotator MoveRotation = FRotator(0.0f, ControllerRotation.Yaw, 0.0f);
		// ��ŷ���ǹ���һ����ת����ȡ�ø���ת�����Ӧ��ķ���
		FVector MoveDirection = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::X);

		// ��������Ҫ�ƶ��ķ����ƶ�ֵ
		AddMovementInput(MoveDirection, Value);
	}
}

void AMainPlayer::MoveRight(float Value)
{
	bool bIsFalling = GetCharacterMovement()->IsFalling();
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsFalling)
	{
		FRotator ControllerRotation = Controller->GetControlRotation();
		FRotator MoveRotation = FRotator(0.0f, ControllerRotation.Yaw, 0.0f);
		FVector MoveDirection = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(MoveDirection, Value);
	}
}

void AMainPlayer::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		float ControlPitch = GetControlRotation().Pitch;
		// �����������
		if (ControlPitch < 270.f && ControlPitch > 180.0f && Value > 0.0f)
		{
			return;
		}
		// ������С����
		if (ControlPitch > 45.0f && ControlPitch < 180.0f && Value < 0.0f)
		{
			return;
		}
		AMainPlayer::AddControllerPitchInput(Value);
	}
}

void AMainPlayer::Turn(float Value)
{
	if (Value != 0.0f)
	{
		AMainPlayer::AddControllerYawInput(Value);
	}
}

void AMainPlayer::Jump()
{
	bool LimitJump = IsTargeting();
	if (!LimitJump)
	{
		Super::Jump();
	}
}

void AMainPlayer::UpdateMovementStatus()
{
	// TODO: crouch
	const FVector PlayerVelocity = GetVelocity();
	PlaneVelocity = FVector(PlayerVelocity.X, PlayerVelocity.Y, 0.0f);

	if (MovemenStatus != EPlayerMovementStatus::EMPS_Target)
	{
		if (PlaneVelocity.Size() > 0.0f)
		{
			SetMovementStatus(EPlayerMovementStatus::EMPS_Jog);
		}
		else
		{
			SetMovementStatus(EPlayerMovementStatus::EMPS_Stand);
		}
	}
}


void AMainPlayer::StartTargeting()
{
	bool LimitTargeting = GetCharacterMovement()->IsFalling();
	if (!LimitTargeting)
	{
		bUseControllerRotationYaw = true;
		CameraSpringArm->SocketOffset = FVector(0, 70, 0);
		CameraSpringArm->TargetOffset = FVector(0, 0, 80);
		CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
		SetMovementStatus(EPlayerMovementStatus::EMPS_Target);
		StartTargetUpdateFOV();
	}
}

void AMainPlayer::EndTargeting()
{
	bUseControllerRotationYaw = false;
	CameraSpringArm->SocketOffset = FVector(0, 0, 0);
	CameraSpringArm->TargetOffset = FVector(0, 0, 0);
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 45.0f));
	SetMovementStatus(EPlayerMovementStatus::EMPS_Stand);
	EndTargetUpdateFOV();
}

void AMainPlayer::SwitchTargeting()
{
	IsTargeting() ? EndTargeting() : StartTargeting();
}

bool AMainPlayer::IsTargeting() const
{
	return MovemenStatus == EPlayerMovementStatus::EMPS_Target ? true : false;
}
