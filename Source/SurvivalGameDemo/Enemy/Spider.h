// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BaseEnemy.h"
#include "Spider.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEDEMO_API ASpider : public ABaseEnemy
{
	GENERATED_BODY()

	ASpider();
	
protected:

	virtual void Attack() override;
};
