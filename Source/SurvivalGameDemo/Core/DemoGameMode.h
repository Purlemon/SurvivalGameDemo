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
	// BeginPlay前在编辑器中指定生成的UI类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Widget")
	TSubclassOf<UUserWidget> MainUIClass;

	// 根据MainUIClass指定的类型持有真正的UI对象
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Widget")
	UUserWidget* MainUI;

protected:
	virtual void BeginPlay()override;
};
