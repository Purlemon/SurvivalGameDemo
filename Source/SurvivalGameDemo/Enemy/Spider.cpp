// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Spider.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

ASpider::ASpider()
{
	LockedMarkMesh->SetWorldLocation(FVector(0.0f, 0.0f, 70.0f));
	
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;

	MaxHealth = 10;
}

void ASpider::Attack()
{
	// Í£Ö¹ÒÆ¶¯
	AIController->StopMovement();

	if (EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking)
	{
		EnemyMovementStatus = EEnemyMovementStatus::EEMS_Attacking;

		// »ñµÃµ±Ç°¹Ç÷ÀµÄ¶¯»­À¶Í¼
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage)
		{
			// ²¥·Å¹¥»÷ÃÉÌ«Ææ
			AnimInstance->Montage_Play(AttackMontage);
			FString SectionName = FString::FromInt(FMath::RandRange(1, 3));
			AnimInstance->Montage_JumpToSection(FName(*SectionName), AttackMontage);
		}
	}
}
