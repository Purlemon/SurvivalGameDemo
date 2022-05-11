// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEDEMO_API UBaseEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Animation Properties")
		float MoveSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Animation Properties")
		class ABaseEnemy* EnemyRef;	// Enemy引用

public:
	// 类似BeginPlay，在动画被创建时调用
	virtual void NativeInitializeAnimation() override;

	// 用于更新动画属性，在蓝图中调用
	UFUNCTION(BlueprintCallable, Category = "Enemy Animation Properties")
		void UpdateAnimationProperties();
};
