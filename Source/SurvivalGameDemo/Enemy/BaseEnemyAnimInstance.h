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
		class ABaseEnemy* EnemyRef;	// Enemy����

public:
	// ����BeginPlay���ڶ���������ʱ����
	virtual void NativeInitializeAnimation() override;

	// ���ڸ��¶������ԣ�����ͼ�е���
	UFUNCTION(BlueprintCallable, Category = "Enemy Animation Properties")
		void UpdateAnimationProperties();
};
