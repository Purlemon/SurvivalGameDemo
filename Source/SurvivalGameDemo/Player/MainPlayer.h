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
	//				 输入
	// -------------------------------------

	float InputX = 0.0f;
	float InputY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float RollingInputX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float RollingInputY = 0.0f;

	// -------------------------------------
	//		    GamePlay相关属性
	// -------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float JogSpeed = 375.0f;	// 跑步速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float RunSpeed = 750.0f;	// 冲刺速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float TargetSpeed = 150.0f;	// 瞄准速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float MaxStamina = 200;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Player State")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaRollConsume = 20.0f;	// 翻滚消耗耐力

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaSlideConsume = 20.0f;	// 滑步消耗耐力

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaConsumeRate = 20.0f;	// 耐力消耗速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	float StaminaRecoverRate = 5.0f;	// 耐力回复速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float ExhaustedStaminaRatio = 0.1f;	// 疲劳区比例

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	EPlayerStaminaStatus StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType HasWeaponType = EWeaponType::EWT_None;	// 持有武器类型

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquippedWeapon;	// 装备的武器

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* OverlappingWeapon;	// 当前可交互的武器

protected:
	// -------------------------------------
	//               移动
	// -------------------------------------

	FTimerHandle RollTimerHandle;	// 翻滚
	bool bRolling = false;	

	FTimerHandle SlideTimerHandle;	// 滑步
	bool bSliding = false;	

	bool bLocking = false;

	FTimerHandle RunLongPressedTimerHandle;
	bool bRunKeyDown = false;	// 标识左shift是否按下

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

	// 每帧调用，设置MovementStatus默认值
	void UpdateMovementStatus(float DeltaTime);

	void UpdateRunStatus(float DeltaTime);

public:
	FORCEINLINE bool IsRolling() { return bRolling; }

	FORCEINLINE bool IsSliding() { return bSliding; }

	FORCEINLINE void SetMovementStatus(EPlayerMovementStatus Status) { MovementStatus = Status; }
	FORCEINLINE EPlayerMovementStatus GetMovementStatus() { return MovementStatus; }

protected:
	// -------------------------------------
	//               瞄准
	// -------------------------------------
	void StartTargeting();
	void EndTargeting();
	void SwitchTargeting();

public:
	UFUNCTION(BlueprintCallable, Category = "Movement Status|Targeting")
	bool IsTargeting() const;

	// 瞄准改变FOV
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement Status|Targeting")
	void StartTargetUpdateFOV();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement Status|Targeting")
	void EndTargetUpdateFOV();

protected:
	// -------------------------------------
	//               交互
	// ------------------------------------- 
	void Interact();
	
};
