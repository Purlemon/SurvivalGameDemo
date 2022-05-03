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
		// 得到player在平面的合速度
		PlayerPlaneVelocity = MainPlayerRef->PlaneVelocity;
		MoveSpeed = PlayerPlaneVelocity.Size();
		PlayerRotation = MainPlayerRef->GetActorRotation();

		bIsInAir = MainPlayerRef->GetMovementComponent()->IsFalling();

		MoveStatus = MainPlayerRef->GetMovementStatus();
	}
}
