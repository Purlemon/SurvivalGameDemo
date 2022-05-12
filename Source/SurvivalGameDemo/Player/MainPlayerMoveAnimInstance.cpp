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
		PlayerPlaneVelocity = MainPlayerRef->PlaneVelocity;
		MoveSpeed = PlayerPlaneVelocity.Size();
		PlayerRotation = MainPlayerRef->GetActorRotation();

		DirectionAngle = CalculateDirection(PlayerPlaneVelocity, MainPlayerRef->GetActorRotation());

		RollingInputX = MainPlayerRef->RollingInputX;
		RollingInputY = MainPlayerRef->RollingInputY;

		bIsInAir = MainPlayerRef->GetMovementComponent()->IsFalling();
		bIsSliding = MainPlayerRef->IsSliding();
		bIsRolling = MainPlayerRef->IsRolling();

		WeaponType = MainPlayerRef->HasWeaponType;
		MoveStatus = MainPlayerRef->GetMovementStatus();
	}
}
