// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EEMS_Idle				UMETA(DisplayName = "Idle"),
	EEMS_MoveToTarget		UMETA(DisplayName = "MoveToTarget"),
	EEMS_Attacking			UMETA(DisplayName = "Attacking"),
	EEMS_Dead				UMETA(DisplayName = "Dead")
};

UCLASS()
class SURVIVALGAMEDEMO_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	UStaticMeshComponent* LockedMarkMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI")
	class USphereComponent* ChaseVolume;	// 触发追逐Player的体积

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI")
	class USphereComponent* AttackVolume;	// 攻击范围

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Widget")
	class UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Widget")
	class UProgressBar* HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Status")
	EEnemyMovementStatus EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;

	// -------------------------------------
	//		    GamePlay相关属性
	// -------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	float MaxMoveSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	int32 Health;

public:
	// -------------------------------------
	//				 移动
	// -------------------------------------
	UFUNCTION()
	void OnChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainPlayer* TargetMainPlayer);

protected:
	// -------------------------------------
	//				 攻击
	// -------------------------------------

	bool bAttackVolumeOverlapping = false;

	AMainPlayer* TargetPlayer;		// 当前正在追逐的Player

protected:

	UFUNCTION()
	void OnAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Attack() {};

public:

	UFUNCTION(BlueprintCallable)
	void AttackEnd();				// 攻击蒙太奇播放结束后调用

protected:
	// -------------------------------------
	//				 伤害
	// -------------------------------------

	void UpdateHealthBar();
};
