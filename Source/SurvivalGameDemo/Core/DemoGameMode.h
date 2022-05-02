// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DemoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEDEMO_API ADemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// BeginPlayǰ�ڱ༭����ָ�����ɵ�UI����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Widget")
	TSubclassOf<UUserWidget> MainUIClass;

	// ����MainUIClassָ�������ͳ���������UI����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Widget")
	UUserWidget* MainUI;

protected:
	virtual void BeginPlay()override;
};
