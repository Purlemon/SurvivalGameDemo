// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DemoGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void ADemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	// �Ѿ�ָ����MainUI����
	if (MainUIClass)
	{
		int32 PlayerIndex = 0; // 0�����
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
		// ������ʹ���ĸ����������ɣ���������
		MainUI = CreateWidget<UUserWidget>(PlayerController, MainUIClass);
		if (MainUI)
		{
			// ��ӵ��ӿ�
			MainUI->AddToViewport();
		}
	}
}