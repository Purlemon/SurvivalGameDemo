// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/MainPlayer.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LockedMarkMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockedMarkMesh"));
	LockedMarkMesh->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("StaticMesh'/Game/Demo/Enemy/S_EnemyLockedMark.S_EnemyLockedMark'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>MaterialAsset(TEXT("Material'/Game/Demo/Enemy/M_EnemyLockedMark.M_EnemyLockedMark'"));
	// 检测是否成功加载
	if (StaticMeshAsset.Succeeded() && MaterialAsset.Succeeded())
	{
		LockedMarkMesh->SetStaticMesh(StaticMeshAsset.Object);
		int32 ElementIndex = 0;	// 材质插槽索引
		LockedMarkMesh->SetMaterial(ElementIndex, MaterialAsset.Object);
	}
	LockedMarkMesh->SetWorldScale3D(FVector(0.1f));
	LockedMarkMesh->SetWorldLocation(FVector(0.0f, 0.0f, 105.0f));
	LockedMarkMesh->SetWorldRotation(FRotator(180.0f, 0.0f, 0.0f));
	LockedMarkMesh->SetVisibility(false);
	LockedMarkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ChaseVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseVolume"));
	ChaseVolume->SetupAttachment(GetRootComponent());
	ChaseVolume->InitSphereRadius(800.0f);
	ChaseVolume->SetCollisionObjectType(ECC_WorldDynamic);
	ChaseVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ChaseVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AttackVolume = CreateDefaultSubobject<USphereComponent>(TEXT("AttackVolume"));
	AttackVolume->SetupAttachment(GetRootComponent());
	AttackVolume->InitSphereRadius(100.0f);
	AttackVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackVolume->SetCollisionObjectType(ECC_WorldDynamic);
	AttackVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);	
	HealthBarWidgetComponent->SetDrawSize(FVector2D(200.0f, 20.0f));

	// 防止挡住摄像机
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// 当被放置在世界或生成时，为Pawn自动创建一个AIController
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	ChaseVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnChaseVolumeOverlapBegin);
	ChaseVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnChaseVolumeOverlapEnd);

	AttackVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnAttackVolumeOverlapBegin);
	AttackVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnAttackVolumeOverlapEnd);

	// 拿到自动创建的AIController
	AIController = Cast<AAIController>(GetController());

	// 拿到自指定的HealthBar
	UUserWidget* HealthBarWidget = HealthBarWidgetComponent->GetUserWidgetObject();
	if (HealthBarWidget)
	{
		HealthBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("HealthBar")));
		if (HealthBar)
		{
			HealthBar->Percent = 1.0f;
			HealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	Health = MaxHealth;
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::OnChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && IsAlive())
		{
			TargetPlayer = MainPlayer;
			MoveToTarget(MainPlayer);

			if (HealthBar)
			{
				HealthBar->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void ABaseEnemy::OnChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && IsAlive())
		{
			EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
			TargetPlayer = nullptr;
			if (AIController)
			{
				AIController->StopMovement();
			}

			if (HealthBar)
			{
				HealthBar->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void ABaseEnemy::OnAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			TargetPlayer = MainPlayer;
			bAttackVolumeOverlapping = true;
			Attack();
		}
	}
}

void ABaseEnemy::OnAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer == TargetPlayer)
		{
			bAttackVolumeOverlapping = false;
		}
	}
}

void ABaseEnemy::MoveToTarget(AMainPlayer* TargetMainPlayer)
{
	EnemyMovementStatus = EEnemyMovementStatus::EEMS_MoveToTarget;

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(TargetMainPlayer);		// 追逐目标
		MoveRequest.SetAcceptanceRadius(10.0f);		// 追到的距离

		AIController->MoveTo(MoveRequest);
	}
}

void ABaseEnemy::AttackEnd()
{
	if (IsAlive())
	{
		EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
		if (bAttackVolumeOverlapping)
		{
			Attack();
		}
		else
		{
			// Player离开攻击范围追逐
			if (TargetPlayer)
			{
				MoveToTarget(TargetPlayer);
			}
		}
	}
}

void ABaseEnemy::UpdateHealthBar()
{
	if (HealthBar)
	{
		float HealthPercent = Health / MaxHealth;
		HealthBar->SetPercent(HealthPercent);
	}
}

float ABaseEnemy::TakeDamage(float AcceptDamage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float CurrentHealth = Health - AcceptDamage;
	if (CurrentHealth <= 0.0f)
	{
		OnDie();
	}
	Health = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	UpdateHealthBar();

	return Health;
}

void ABaseEnemy::OnDie()
{
	EnemyMovementStatus = EEnemyMovementStatus::EEMS_Dead;

	ChaseVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBar->SetVisibility(ESlateVisibility::Hidden);

	if (TargetPlayer && TargetPlayer->IsEnemyLocking(this))
	{
		TargetPlayer->EndLocking();
	}
}

void ABaseEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle DeathTimerHandle;
	const auto DeathAndDestroy = [this]() { Destroy(); };
	float DelayTime = 1.0f;
	bool bLoop = false;
	GetWorldTimerManager().SetTimer(DeathTimerHandle, FTimerDelegate::CreateLambda(DeathAndDestroy), DelayTime, bLoop);
}

