// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayerMoveAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainPlayerMoveAnimInstance::NativeInitializeAnimation()
{
	MainPlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
}

void UMainPlayerMoveAnimInstance::UpdateAnimationProperties()
{
	if (MainPlayerRef)
	{
		// �õ�player��ƽ��ĺ��ٶ�
		FVector PlayerSpeed = MainPlayerRef->GetVelocity();
		FVector PlayerPlanarSpeed = FVector(PlayerSpeed.X, PlayerSpeed.Y, 0.0f);
		MoveSpeed = PlayerPlanarSpeed.Size();

		bIsInAir = MainPlayerRef->GetMovementComponent()->IsFalling();

		bTargeting = MainPlayerRef->IsTargeting();
	}
}
