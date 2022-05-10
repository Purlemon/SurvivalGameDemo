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
	class USphereComponent* ChaseVolume;	// ´¥·¢×·ÖðPlayerµÄÌå»ý

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI")
	class USphereComponent* AttackVolume;			// ¹¥»÷·¶Î§

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy AI")
	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Status")
	EEnemyMovementStatus EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;

public:

	UFUNCTION()
	void OnChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainPlayer* TargetPlayer);
};
