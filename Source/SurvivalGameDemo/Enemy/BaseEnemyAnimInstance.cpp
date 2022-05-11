// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemyAnimInstance.h"
#include "Enemy/BaseEnemy.h"

void UBaseEnemyAnimInstance::NativeInitializeAnimation()
{
	EnemyRef = Cast<ABaseEnemy>(TryGetPawnOwner());
}

void UBaseEnemyAnimInstance::UpdateAnimationProperties()
{
	if (EnemyRef != nullptr)
	{
		// 得到player在平面的合速度
		FVector PlayerSpeed = EnemyRef->GetVelocity();
		FVector PlayerPlanarSpeed = FVector(PlayerSpeed.X, PlayerSpeed.Y, 0.0f);
		MoveSpeed = PlayerPlanarSpeed.Size();
	}
}
