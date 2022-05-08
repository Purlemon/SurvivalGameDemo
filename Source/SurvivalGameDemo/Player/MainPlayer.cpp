// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetRootComponent());
	CameraSpringArm->TargetArmLength = 350.0f;	// 悬臂长度
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 45.0f));
	CameraSpringArm->bUsePawnControlRotation = true;	// 设置悬臂被Controller控制转向

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	// -------------------------------------
	//				 移动
	// -------------------------------------

	bUseControllerRotationYaw = false;	// 取消Player被Controller控制转向
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.5f;		  // 设置重力
	MoveComp->JumpZVelocity = 600.0f;		  // 设置起跳速度
	MoveComp->bOrientRotationToMovement = true;	// 自动转身
	MoveComp->RotationRate = FRotator(0.0f, 800.0f, 0.0f);// 转身速率

	// -------------------------------------
	//			GamePlay状态
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

	// 移动
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainPlayer::MoveRight);

	// 视角跟随鼠标转动
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainPlayer::LookUp);

	// 冲刺
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AMainPlayer::RunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AMainPlayer::RunReleased);

	// 翻滚
	PlayerInputComponent->BindAction(TEXT("Roll"), IE_Pressed, this, &AMainPlayer::Roll);

	// 跳跃
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainPlayer::Jump);

	// 交互
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AMainPlayer::Interact);

	// 锁定
	PlayerInputComponent->BindAction(TEXT("Lock"), IE_Pressed, this, &AMainPlayer::Lock);

	// 武器
	// 枪械瞄准
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Pressed, this, &AMainPlayer::StartTargeting);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Released, this, &AMainPlayer::EndTargeting);
	PlayerInputComponent->BindAction(TEXT("LongTargeting"), IE_Pressed, this, &AMainPlayer::SwitchTargeting);

}

void AMainPlayer::MoveForward(float Value)
{
	bool bLimitMove = GetCharacterMovement()->IsFalling() || IsRolling() || IsSliding();
	if ((Controller != nullptr) && (Value != 0.0f) && !bLimitMove)
	{
		// 得到Controller的旋转角
		FRotator ControllerRotation = Controller->GetControlRotation();
		FRotator MoveRotation = FRotator(0.0f, ControllerRotation.Yaw, 0.0f);
		// 从欧拉角构造一个旋转矩阵，取得该旋转矩阵对应轴的方向
		FVector MoveDirection = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::X);

		// 参数：将要移动的方向，移动值
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
		// 限制最大仰角
		if (ControlPitch < 270.f && ControlPitch > 180.0f && Value > 0.0f)
		{
			return;
		}
		// 限制最小俯角
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
	// 长按冲刺
	bool bLoop = false;
	float LongPressedTime = 0.2f;
	const auto StartRunning = [this]() { RunKeyDown(); };
	GetWorldTimerManager().SetTimer(RunLongPressedTimerHandle, FTimerDelegate::CreateLambda(StartRunning), LongPressedTime, bLoop);
}

void AMainPlayer::RunReleased()
{
	// 长按结束冲刺，短按滑步
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
		// 翻滚时间自动结束
		float InputValue = FMath::Abs(InputX) + FMath::Abs(InputY);
		if (InputValue > 0.0f)
		{
			if (bLocking)
			{
				RollingInputX = InputX;
				RollingInputY = InputY;
			} 
			else
			{	// 无锁定向角色面向翻滚
				RollingInputX = 0.0f;
				RollingInputY = 1.0f;
			}
			float RollingTime = 0.6f;
			GetWorldTimerManager().SetTimer(RollTimerHandle, FTimerDelegate::CreateLambda(EndRoll), RollingTime, bLoop);
		}
		else  
		{	// 原地后撤步
			RollingInputX = 0.0f;
			RollingInputY = 0.0f;
			float RollingTime = 0.2f;
			GetWorldTimerManager().SetTimer(RollTimerHandle, FTimerDelegate::CreateLambda(EndRoll), RollingTime, bLoop);
		}
		bRolling = true;

		// 退出瞄准
		EndTargeting();

		// 消耗耐力
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

		// 退出瞄准
		EndTargeting();

		// 消耗耐力
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

			// 没有速度为站立，有速度才判断是不是冲刺
			if (PlaneVelocity.Size() > 0.0f)
			{
				// 默认状态
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
				// 冲刺消耗耐力
				Stamina -= StaminaConsume;
				if (Stamina <= ExhaustedValue)
				{
					StaminaStatus = EPlayerStaminaStatus::EPSS_Exhausted;
				}
				SetMovementStatus(EPlayerMovementStatus::EMPS_Run);
			} 
			else
			{
				// 恢复耐力
				Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			}
			break;
		}
		case EPlayerStaminaStatus::EPSS_Exhausted: {
			if (bRunKeyDown)
			{
				// 在疲劳区继续冲刺
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
				// 在疲劳区结束冲刺
				StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
				// 恢复耐力
				Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			}
			break;
		}
		case EPlayerStaminaStatus::EPSS_ExhaustedRecovering: {
			// 疲劳回复期，只能恢复耐力
			Stamina = FMath::Min(Stamina + StaminaRecover, MaxStamina);
			if (Stamina >= ExhaustedValue)
			{	
				// 结束疲劳恢复状态
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
	if (OverlappingWeapon)	// 在交互范围内装备新武器
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->UnEquip(this);
		}
		OverlappingWeapon->Equip(this);
	} 
	else	// 卸下武器
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->UnEquip(this);
		}
	}
}

void AMainPlayer::Lock()
{
	if (bLocking)
	{
		EndLocking();
	} 
	else
	{	
		bool bHasEnemies = FindAndUpdateCanLockedEnemies();

		if (bHasEnemies)
		{
			// 得到距离视野中线最近的敌人
			CanLockedEnemies.ValueSort([](const FEnemyRelativePlayerInfo& A, const FEnemyRelativePlayerInfo& B) {
				return A.AbsEnemyRelativePlayerYaw < B.AbsEnemyRelativePlayerYaw;
				});
			ABaseEnemy* LockedEnemy = CanLockedEnemies.begin().Key();
			
			{	// 检查Player和该敌人之间在一定宽度内是否有其他敌人，若有则就近锁定
				const FVector StartLocation = GetActorLocation();
				const FVector EndLocation = LockedEnemy->GetActorLocation();
				float SphereRadius = 25.0f;
				TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
				bool bTraceComplex = false;
				const TArray<AActor*> ActorsToIgnore{ LockedEnemy };
				EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
				FHitResult OutHit;
				bool bIgnoreSelf = true;

				UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartLocation, EndLocation, SphereRadius, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
			
				ABaseEnemy* HitEnemy = Cast<ABaseEnemy>(OutHit.Actor);
				if (HitEnemy)
				{
					LockedEnemy = HitEnemy;
				}
			}
			
			StartLocking(LockedEnemy);
		}
	}
}

bool AMainPlayer::FindAndUpdateCanLockedEnemies()
{
	CanLockedEnemies.Reset();

	// 取得所有敌人
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), OutActors);

	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FRotator RotationFromPlayer = UKismetMathLibrary::FindLookAtRotation(CameraLocation, GetActorLocation());

	// 检测每个敌人是否可锁定
	for (auto& Actor : OutActors)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor);
		if (Enemy && IsActorInLockDistance(Enemy) && IsActorInSight(Enemy))
		{
			// 计算每个敌人与视野中线的夹角
			const FRotator RotationFromEnemy = UKismetMathLibrary::FindLookAtRotation(CameraLocation, Enemy->GetActorLocation());
			float EnemyRelativePlayerYaw = RotationFromEnemy.Yaw - RotationFromPlayer.Yaw;
			float AbsEnemyRelativePlayerYaw = FMath::Abs(EnemyRelativePlayerYaw);
			CanLockedEnemies.Add(Enemy, {EnemyRelativePlayerYaw, AbsEnemyRelativePlayerYaw });
		}
	}

	return CanLockedEnemies.Num() > 0 ? true : false;
}

void AMainPlayer::StartLocking(ABaseEnemy* LockedEnemy)
{
	bLocking = true;
	if (LockedEnemy)
	{
		CurrentLockingEnemy = LockedEnemy;
		CurrentLockingEnemy->LockedMarkMesh->SetVisibility(true);
	}
}

void AMainPlayer::EndLocking()
{
	bLocking = false;
	if (CurrentLockingEnemy)
	{
		CurrentLockingEnemy->LockedMarkMesh->SetVisibility(false);
	}
}

bool AMainPlayer::IsActorInLockDistance(AActor* OtherActor)
{
	return GetDistanceTo(OtherActor) <= MaxLockDistance ? true : false;
}

bool AMainPlayer::IsActorInSight(AActor* OtherActor)
{
	// 从角色向目标Actor发出射线，检测路上有没有障碍物
	if (OtherActor)
	{
		const FVector StartLocation = GetActorLocation();	// TODO:使用相机位置做更合理的检测
		const FVector EndLocation = OtherActor->GetActorLocation();
		TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		bool bTraceComplex = false;
		const TArray<AActor*> ActorsToIgnore{ this };
		EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
		FHitResult OutHit;
		bool bIgnoreSelf = true;

		bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLocation, EndLocation, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
		
		return !bHit;
	}

	return false;
}
