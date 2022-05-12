// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LongSword.h"
#include "Components/BoxComponent.h"

ALongSword::ALongSword()
{
	Type = EWeaponType::EWT_LongSword;
	SocketName = TEXT("LongSwordSocket");

	DamageValue = 3.0f;

	SocketScale = FVector(1.0f);	// 仅用于Socket调整缩放失效
}
