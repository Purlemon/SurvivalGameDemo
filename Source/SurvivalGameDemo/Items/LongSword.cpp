// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LongSword.h"

ALongSword::ALongSword()
{
	Type = EWeaponType::EWT_LongSword;
	SocketName = TEXT("LongSwordSocket");
	SocketScale = FVector(1.0f);	// ������Socket��������ʧЧ
}
