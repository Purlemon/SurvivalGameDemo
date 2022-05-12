// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_None UMETA(DisplayName = "None"),
	EWT_LongSword UMETA(DisplayName = "LongSword"),
	EWT_Katana UMETA(DisplayName = "Katana"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_CanPickedup UMETA(DisplayName = "CanPickedup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
};

UCLASS(ABSTRACT)
class SURVIVALGAMEDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	// Sets default values for this actor's properties
	AWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh; 		// ��ʾ�������ģ��

	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerSphere;	// ����������ײ��

	UPROPERTY(EditAnywhere, Category = "Weapon|Sound")
	USoundCue* OnEquipedSound;			// װ��ʱ����Ч

	UPROPERTY(EditAnywhere, Category = "Weapon|State")
	EWeaponState State = EWeaponState::EWS_CanPickedup;	

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	EWeaponType Type;

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	bool bNeedRotate = true;

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float RotationRate = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	FVector SocketScale = FVector(1.0f);	// ������Socket��������ʧЧ

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ActiveWeaponCollision();
	void DeactiveWeaponCollision();

public:
	void Equip(class AMainPlayer* MainPlayer);
	void UnEquip(AMainPlayer* MainPlayer);

public:
	// -------------------------------------
	//				�˺�
	// -------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Attack")
	class UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	float DamageValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Attack")
	class AController* WeaponOwner;

protected:

	UFUNCTION()
	void OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	// ���ڹ���ʱ������ײ����
	UFUNCTION(BlueprintCallable)
	void ActiveHitBox();

	UFUNCTION(BlueprintCallable)
	void DeactiveHitBox();

};
