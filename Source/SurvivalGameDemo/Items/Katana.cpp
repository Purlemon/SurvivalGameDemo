// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Katana.h"

AKatana::AKatana()
{
	Type = EWeaponType::EWT_Katana;
	SocketName = TEXT("KatanaSocket");

	DamageValue = 2.0f;

	SocketScale = FVector(1.0f);	// 仅用于Socket调整缩放失效
}
