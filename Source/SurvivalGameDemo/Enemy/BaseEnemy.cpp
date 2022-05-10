// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/MainPlayer.h"
#include "AIController.h"

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
		if (MainPlayer)
		{
			MoveToTarget(MainPlayer);
		}
	}
}

void ABaseEnemy::OnChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void ABaseEnemy::OnAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ABaseEnemy::OnAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseEnemy::MoveToTarget(class AMainPlayer* TargetPlayer)
{
	EnemyMovementStatus = EEnemyMovementStatus::EEMS_MoveToTarget;

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(TargetPlayer);		// 追逐目标
		MoveRequest.SetAcceptanceRadius(10.0f);		// 追到的距离

		AIController->MoveTo(MoveRequest);
	}
}

