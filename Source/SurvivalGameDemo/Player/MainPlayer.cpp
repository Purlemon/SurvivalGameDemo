// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "Kismet/KismetMathLibrary.h"
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

	// -------------------------------------
	//				 �ƶ�
	// -------------------------------------

	bUseControllerRotationYaw = false;	// ȡ��Player��Controller����ת��
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.5f;		  // ��������
	MoveComp->JumpZVelocity = 600.0f;		  // ���������ٶ�
	MoveComp->bOrientRotationToMovement = true;	// �Զ�ת��
	MoveComp->RotationRate = FRotator(0.0f, 800.0f, 0.0f);// ת������

	// -------------------------------------
	//			GamePlay״̬
	// -------------------------------------

	Health = MaxHealth;
	Stamina = MaxStamina;

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

	UpdateMovementStatus(DeltaTime);
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

	// ���
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AMainPlayer::RunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AMainPlayer::RunReleased);

	// ����
	PlayerInputComponent->BindAction(TEXT("Roll"), IE_Pressed, this, &AMainPlayer::Roll);

	// ��Ծ
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainPlayer::Jump);

	// ����
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AMainPlayer::Interact);

	// ����
	// ǹе��׼
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Pressed, this, &AMainPlayer::StartTargeting);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Released, this, &AMainPlayer::EndTargeting);
	PlayerInputComponent->BindAction(TEXT("LongTargeting"), IE_Pressed, this, &AMainPlayer::SwitchTargeting);

}

void AMainPlayer::MoveForward(float Value)
{
	bool bLimitMove = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding();
	if ((Controller != nullptr) && (Value != 0.0f) && !bLimitMove)
	{
		// �õ�Controller����ת��
		FRotator ControllerRotation = Controller->GetControlRotation();
		FRotator MoveRotation = FRotator(0.0f, ControllerRotation.Yaw, 0.0f);
		// ��ŷ���ǹ���һ����ת����ȡ�ø���ת�����Ӧ��ķ���
		FVector MoveDirection = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::X);

		// ��������Ҫ�ƶ��ķ����ƶ�ֵ
		AddMovementInput(MoveDirection, Value);
	}

	InputY = Value;
}

void AMainPlayer::MoveRight(float Value)
{
	bool bLimitMove = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding();
	if ((Controller != nullptr) && (Value != 0.0f) && !bLimitMove)
	{
		FRotator ControllerRotation = Controller->GetControlRotation();
		FRotator MoveRotation = FRotator(0.0f, ControllerRotation.Yaw, 0.0f);
		FVector MoveDirection = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(MoveDirection, Value);
	}
	InputX = Value;
}

void AMainPlayer::LookUp(float Value)
{
	bool bLimitLookUp = bLocking;
	if (Value != 0.0f && !bLimitLookUp)
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
	bool bLimitTurn = bLocking;
	if (Value != 0.0f && !bLimitTurn)
	{
		AMainPlayer::AddControllerYawInput(Value);
	}
}

void AMainPlayer::RunPressed()
{
	// �������
	bool bLoop = false;
	float LongPressedTime = 0.2f;
	const auto StartRunning = [this]() { RunKeyDown(); };
	GetWorldTimerManager().SetTimer(RunLongPressedTimerHandle, FTimerDelegate::CreateLambda(StartRunning), LongPressedTime, bLoop);
}

void AMainPlayer::RunReleased()
{
	// ����������̣��̰�����
	if (bRunKeyDown)
	{
		RunKeyUp();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RunLongPressedTimerHandle);
		Slide();
	}
}

void AMainPlayer::Roll()
{
	bool bLimitRoll = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding() || Stamina < StaminaRollConsume;
	if (!bLimitRoll)
	{
		bool bLoop = false;
		const auto EndRoll = [this]() { bRolling = false; };
		// ����ʱ���Զ�����
		float InputValue = FMath::Abs(InputX) + FMath::Abs(InputY);
		if (InputValue > 0.0f)
		{
			if (bLocking)
			{
				RollingInputX = InputX;
				RollingInputY = InputY;
			} 
			else
			{	// ���������ɫ���򷭹�
				RollingInputX = 0.0f;
				RollingInputY = 1.0f;
			}
			float RollingTime = 0.6f;
			GetWorldTimerManager().SetTimer(RollTimerHandle, FTimerDelegate::CreateLambda(EndRoll), RollingTime, bLoop);
		}
		else  
		{	// ԭ�غ󳷲�
			RollingInputX = 0.0f;
			RollingInputY = 0.0f;
			float RollingTime = 0.2f;
			GetWorldTimerManager().SetTimer(RollTimerHandle, FTimerDelegate::CreateLambda(EndRoll), RollingTime, bLoop);
		}
		bRolling = true;

		// �˳���׼
		EndTargeting();

		// ��������
		Stamina -= StaminaRollConsume;
	}
}

void AMainPlayer::Slide()
{
	bool bLimitRoll = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding() || Stamina < StaminaSlideConsume;
	if (!bLimitRoll)
	{
		bool bLoop = false;
		const auto EndSlide = [this]() { bSliding = false; };
		float SlideTime = 0.4f;
		GetWorldTimerManager().SetTimer(RollTimerHandle, FTimerDelegate::CreateLambda(EndSlide), SlideTime, bLoop);
		
		bSliding = true;

		// �˳���׼
		EndTargeting();

		// ��������
		Stamina -= StaminaSlideConsume;
	}
}

void AMainPlayer::Jump()
{
	bool bLimitJump = IsTargeting() || IsRolling() || IsSliding();
	if (!bLimitJump)
	{
		Super::Jump();
	}
}

void AMainPlayer::UpdateMovementStatus(float DeltaTime)
{
	// TODO: crouch

	switch (MovementStatus)
	{
		case EPlayerMovementStatus::EMPS_Target:{
			GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;

			float StaminaRecover = StaminaRecoverRate * DeltaTime;
			Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			break;
		}
		case EPlayerMovementStatus::EMPS_Crouch:{
			break;
		}
		default:{
			const FVector PlayerVelocity = GetVelocity();
			PlaneVelocity = FVector(PlayerVelocity.X, PlayerVelocity.Y, 0.0f);

			// û���ٶ�Ϊվ�������ٶȲ��ж��ǲ��ǳ��
			if (PlaneVelocity.Size() > 0.0f)
			{
				// Ĭ��״̬
				SetMovementStatus(EPlayerMovementStatus::EMPS_Jog);
				UpdateRunStatus(DeltaTime);

				if (MovementStatus == EPlayerMovementStatus::EMPS_Jog)
				{
					GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
				}
				else
				{
					GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
				}
			} 
			else
			{
				SetMovementStatus(EPlayerMovementStatus::EMPS_Stand);

				float StaminaRecover = StaminaRecoverRate * DeltaTime * 4;
				Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			}
			break;
		}
	}

}

void AMainPlayer::UpdateRunStatus(float DeltaTime)
{
	float StaminaConsume = StaminaConsumeRate * DeltaTime;
	float StaminaRecover = StaminaRecoverRate * DeltaTime;
	float ExhaustedValue = MaxStamina * ExhaustedStaminaRatio;

	switch (StaminaStatus)
	{
		case EPlayerStaminaStatus::EPSS_Normal: {
			if (bRunKeyDown)
			{
				// �����������
				Stamina -= StaminaConsume;
				if (Stamina <= ExhaustedValue)
				{
					StaminaStatus = EPlayerStaminaStatus::EPSS_Exhausted;
				}
				SetMovementStatus(EPlayerMovementStatus::EMPS_Run);
			} 
			else
			{
				// �ָ�����
				Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			}
			break;
		}
		case EPlayerStaminaStatus::EPSS_Exhausted: {
			if (bRunKeyDown)
			{
				// ��ƣ�����������
				Stamina = FMath::Max(Stamina - StaminaRecover, 0.0f);
				if (Stamina <= 0)
				{
					StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
					RunKeyUp();
				} 
				else
				{
					SetMovementStatus(EPlayerMovementStatus::EMPS_Run);
				}
			} 
			else
			{
				// ��ƣ�����������
				StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
				// �ָ�����
				Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			}
			break;
		}
		case EPlayerStaminaStatus::EPSS_ExhaustedRecovering: {
			// ƣ�ͻظ��ڣ�ֻ�ָܻ�����
			Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			if (Stamina >= ExhaustedValue)
			{	
				// ����ƣ�ͻָ�״̬
				StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;
			}
			break;
		}
		default:{
			break;
		}
	}
}

void AMainPlayer::StartTargeting()
{
	bool bLimitTarget = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding();
	if (!bLimitTarget)
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
	return MovementStatus == EPlayerMovementStatus::EMPS_Target ? true : false;
}

void AMainPlayer::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Interact"));
	if (OverlappingWeapon)	// �ڽ�����Χ��װ��������
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Overlapping Weapon"));
		if (EquippedWeapon)
		{
			EquippedWeapon->UnEquip(this);
		}
		UE_LOG(LogTemp, Warning, TEXT("Player Equipped Weapon"));
		OverlappingWeapon->Equip(this);
	} 
	else	// ж������
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->UnEquip(this);
		}
	}
}
