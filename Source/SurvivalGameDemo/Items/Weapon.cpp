// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Player/MainPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy/BaseEnemy.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 设置为在第一次拾取前以可交互物品的形式呈现
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(GetRootComponent());

	// 设置碰撞预设为仅Pawn触发
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 将攻击box附着在武器上的socket，根据socket名字索引
	const FName HitBoxSocketName = FName(TEXT("HitBoxSocket"));
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(Mesh, HitBoxSocketName);
	DeactiveHitBox();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnHitBoxOverlapBegin);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnHitBoxOverlapEnd);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bNeedRotate)
	{
		FRotator NewRotator = GetActorRotation();
		NewRotator.Yaw += RotationRate * DeltaTime;
		SetActorRotation(NewRotator);
	}
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 玩家进入交互范围时，将玩家可交互武器设为this
	if (OtherActor && State == EWeaponState::EWS_CanPickedup)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->OverlappingWeapon = this;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin"));
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 玩家离开可交互范围时，如果玩家可交互武器为this则设为空
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && MainPlayer->OverlappingWeapon == this)
		{
			MainPlayer->OverlappingWeapon = nullptr;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd"));
}

void AWeapon::Equip(AMainPlayer* MainPlayer)
{
	if (MainPlayer)
	{
		State = EWeaponState::EWS_Equipped;
		WeaponOwner = MainPlayer->GetController();

		DeactiveWeaponCollision();

		// 得到武器在Player骨骼上的插槽
		const USkeletalMeshSocket* Socket = MainPlayer->GetMesh()->GetSocketByName(SocketName);
		if (Socket)
		{
			// 将武器附着到插槽上
			Socket->AttachActor(this, MainPlayer->GetMesh());

			// Player状态改变
			MainPlayer->HasWeaponType = Type;
			MainPlayer->EquippedWeapon = this;
			MainPlayer->OverlappingWeapon = nullptr;
			MainPlayer->UpdateAttackMontage(Type);

			// 武器状态改变
			bNeedRotate = false;
			// Socket参数失效时在这里改变缩放
			SetActorScale3D(SocketScale);

			if (OnEquipedSound)
			{
				UGameplayStatics::PlaySound2D(this, OnEquipedSound);
			}
		}
	}
}

void AWeapon::UnEquip(AMainPlayer* MainPlayer)
{
	if (MainPlayer)
	{
		bool bLimitUnEquip = MainPlayer->GetMovementComponent()->IsFalling();
		if (!bLimitUnEquip)
		{
			State = EWeaponState::EWS_CanPickedup;
			WeaponOwner = nullptr;

			ActiveWeaponCollision();

			// Player状态改变
			MainPlayer->HasWeaponType = EWeaponType::EWT_None;
			MainPlayer->EquippedWeapon = nullptr;
			// 如果当前Player不在其他武器的交互范围内，则设为this
			if (MainPlayer->OverlappingWeapon == nullptr)
			{
				MainPlayer->OverlappingWeapon = this;
			}

			// 卸下时保持世界坐标下的变换
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AWeapon::ActiveWeaponCollision()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetSimulatePhysics(true);

	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWeapon::DeactiveWeaponCollision()
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(OtherActor);
		if (BaseEnemy)
		{
			// 对敌人造成伤害
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(BaseEnemy, DamageValue, WeaponOwner, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon::OnHitBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActiveHitBox()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWeapon::DeactiveHitBox()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

