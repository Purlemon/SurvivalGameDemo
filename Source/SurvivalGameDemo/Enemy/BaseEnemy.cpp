// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"

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
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

