// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Items/Weapon.h"
#include "MainPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementStatus : uint8
{
	EMPS_Stand UMETA(DisplayName = "Stand"),
	EMPS_Crouch UMETA(DisplayName = "Crouch"),
	EMPS_Jog UMETA(DisplayName = "Jog"),
	EMPS_Run UMETA(DisplayName = "Run"),
	EMPS_Target UMETA(DisplayName = "Target")
};

UENUM(BlueprintType)
enum class EPlayerStaminaStatus : uint8
{
	EPSS_Normal UMETA(DisplayName = "Normal"),
	EPSS_Exhausted UMETA(DisplayName = "Exhausted"),
	EPSS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering")
};

//USTRUCT()
//struct FEnemyRelativePlayerInfo
//{
//	GENERATED_USTRUCT_BODY()
//
//	float EnemyRelativePlayerYaw;
//	float AbsEnemyRelativePlayerYaw;
//};

UCLASS()
class SURVIVALGAMEDEMO_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	EPlayerMovementStatus MovementStatus = EPlayerMovementStatus::EMPS_Stand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	FVector PlaneVelocity = FVector(0.0f);

	// -------------------------------------
	//				 ����
	// -------------------------------------

	float InputX = 0.0f;
	float InputY = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Input")
	float InputLookUp = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Input")
	float InputTurn = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Input")
	float RollingInputX = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Input")
	float RollingInputY = 0.0f;

	// -------------------------------------
	//		    GamePlay�������
	// -------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float JogSpeed = 375.0f;	// �ܲ��ٶ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float RunSpeed = 750.0f;	// ����ٶ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float TargetSpeed = 150.0f;	// ��׼�ٶ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float MaxStamina = 200;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Player State")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaRollConsume = 20.0f;	// ������������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaSlideConsume = 20.0f;	// ������������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaConsumeRate = 20.0f;	// ���������ٶ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaRecoverRate = 5.0f;	// �����ظ��ٶ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float ExhaustedStaminaRatio = 0.1f;	// ƣ��������

protected:
	// -------------------------------------
	//               �ƶ�
	// -------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	EPlayerStaminaStatus StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;

	FTimerHandle RollTimerHandle;	// ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bRolling = false;	

	FTimerHandle SlideTimerHandle;	// ����
	bool bSliding = false;	

	FTimerHandle RunLongPressedTimerHandle;
	bool bRunKeyDown = false;	// ��ʶ��shift�Ƿ���

protected:

	void MoveForward(float Value);
	void MoveRight(float Value);

	void LookUp(float Value);
	void Turn(float Value);

	void RunPressed();
	void RunReleased();

	FORCEINLINE void RunKeyDown() { bRunKeyDown = true; }
	FORCEINLINE void RunKeyUp() { bRunKeyDown = false; }

	void Roll();
	void Slide();

	virtual void Jump()override;

	// ÿ֡���ã�����MovementStatusĬ��ֵ
	void UpdateMovementStatus(float DeltaTime);

	void UpdateRunStatus(float DeltaTime);

public:
	FORCEINLINE bool IsRolling() { return bRolling; }

	FORCEINLINE bool IsSliding() { return bSliding; }

	FORCEINLINE void SetMovementStatus(EPlayerMovementStatus Status) { MovementStatus = Status; }
	FORCEINLINE EPlayerMovementStatus GetMovementStatus() { return MovementStatus; }

protected:
	// -------------------------------------
	//               ��׼
	// -------------------------------------

	void StartTargeting();
	void EndTargeting();
	void SwitchTargeting();

public:
	UFUNCTION(BlueprintCallable, Category = "Movement Status|Targeting")
	bool IsTargeting() const;

	// ��׼�ı�FOV
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement Status|Targeting")
	void StartTargetUpdateFOV();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement Status|Targeting")
	void EndTargetUpdateFOV();

public:
	// -------------------------------------
	//               ����
	// ------------------------------------- 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType HasWeaponType = EWeaponType::EWT_None;	// ������������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquippedWeapon;	// װ��������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* OverlappingWeapon;	// ��ǰ�ɽ���������

protected:

	void Interact();
	
protected:
	// -------------------------------------
	//             ����ϵͳ
	// ------------------------------------- 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	bool bLocking = false;

	float MaxLockDistance = 1500.0f;

	UPROPERTY(VisibleAnywhere, Category = "Lock System")
	TMap<class ABaseEnemy*, float> CanLockedEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock System")
	ABaseEnemy* CurrentLockingEnemy;

protected:	
	void Lock();

	void SwitchLockedLeft();
	void SwitchLockedRight();
	void SwitchLocked(bool bIsLeft);

	// �ҵ���ǰ�������������ĵ��ˣ�����CanLockedEnemies
	bool FindAndUpdateCanLockedEnemies();

	void StartLocking(ABaseEnemy* LockedEnemy);
	void EndLocking();

	bool IsActorInLockDistance(AActor* OtherActor);
	bool IsActorInSight(AActor* OtherActor);
	
	// ÿ֡���ã���������״̬
	void UpdateLockingStatus(float DeltaTime);

	// �����ڼ�������������
	void UpdateLockingCameraRotation(float DeltaTime);

	// �����ڼ䷭������ɫ�������
	void UpdateLockingActorRotation(float DeltaTime);

	// ��������ʱ���������Picth������ͼ��ʵ��
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock System")
	void ResetCameraAfterEndLocking();
};
