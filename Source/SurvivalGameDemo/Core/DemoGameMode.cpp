// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DemoGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void ADemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 已经指定了MainUI类型
	if (MainUIClass)
	{
		int32 PlayerIndex = 0; // 0号玩家
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
		// 参数：使用哪个控制器生成，生成类型
		MainUI = CreateWidget<UUserWidget>(PlayerController, MainUIClass);
		if (MainUI)
		{
			// 添加到视口
			MainUI->AddToViewport();
		}
	}
}