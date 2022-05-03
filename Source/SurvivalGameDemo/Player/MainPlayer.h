// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
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
	EPlayerMovementStatus MovemenStatus = EPlayerMovementStatus::EMPS_Stand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
	FVector PlaneVelocity = FVector(0.0f);

	// -------------------------------------
	//		    GamePlay相关属性
	// -------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player State")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player State")
	float Health;

protected:
	// -------------------------------------
	//               移动
	// -------------------------------------
	void MoveForward(float Value);
	void MoveRight(float Value);

	void LookUp(float Value);
	void Turn(float Value);

	virtual void Jump()override;

	// 每帧调用，设置MovementStatus默认值
	void UpdateMovementStatus();

public:
	FORCEINLINE void SetMovementStatus(EPlayerMovementStatus Status) { MovemenStatus = Status; }
	FORCEINLINE EPlayerMovementStatus GetMovementStatus() { return MovemenStatus; }

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
};
