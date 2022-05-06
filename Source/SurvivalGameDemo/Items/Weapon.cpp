// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Player/MainPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// ����Ϊ�ڵ�һ��ʰȡǰ�Կɽ�����Ʒ����ʽ����
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(GetRootComponent());

	// ������ײԤ��Ϊ��Pawn����
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);
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
	// ��ҽ��뽻����Χʱ������ҿɽ���������Ϊthis
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
	// ����뿪�ɽ�����Χʱ�������ҿɽ�������Ϊthis����Ϊ��
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

		DeactiveDisplayMeshCollision();

		// �õ�������Player�����ϵĲ��
		const USkeletalMeshSocket* Socket = MainPlayer->GetMesh()->GetSocketByName(SocketName);
		if (Socket)
		{
			// ���������ŵ������
			Socket->AttachActor(this, MainPlayer->GetMesh());

			// Player״̬�ı�
			MainPlayer->HasWeaponType = Type;
			MainPlayer->EquippedWeapon = this;
			MainPlayer->OverlappingWeapon = nullptr;

			// ����״̬�ı�
			bNeedRotate = false;
			// Socket����ʧЧʱ������ı�����
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

			ActiveDisplayMeshCollision();

			// Player״̬�ı�
			MainPlayer->HasWeaponType = EWeaponType::EWT_None;
			MainPlayer->EquippedWeapon = nullptr;
			// �����ǰPlayer�������������Ľ�����Χ�ڣ�����Ϊthis
			if (MainPlayer->OverlappingWeapon == nullptr)
			{
				MainPlayer->OverlappingWeapon = this;
			}

			// ж��ʱ�������������µı任
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AWeapon::ActiveDisplayMeshCollision()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
}

void AWeapon::DeactiveDisplayMeshCollision()
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
