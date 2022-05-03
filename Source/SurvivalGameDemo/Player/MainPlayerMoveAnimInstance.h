// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Player/MainPlayer.h"
#include "MainPlayerMoveAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEDEMO_API UMainPlayerMoveAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	float MoveSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	FVector PlayerPlaneVelocity = FVector(0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	FRotator PlayerRotation = FRotator(0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	EPlayerMovementStatus MoveStatus = EPlayerMovementStatus::EMPS_Stand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation Properties")
	AMainPlayer* MainPlayerRef;
	
public:
	// 类似BeginPlay，在动画被创建时调用
	virtual void NativeInitializeAnimation() override;

	// 用于更新动画属性，在蓝图中调用
	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties();
};
