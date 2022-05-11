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
		// �õ�player��ƽ��ĺ��ٶ�
		FVector PlayerSpeed = EnemyRef->GetVelocity();
		FVector PlayerPlanarSpeed = FVector(PlayerSpeed.X, PlayerSpeed.Y, 0.0f);
		MoveSpeed = PlayerPlanarSpeed.Size();
	}
}
