// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "InteractableItem.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAMEDEMO_API AInteractableItem : public AActor
{
	GENERATED_BODY()

protected:
	// Sets default values for this actor's properties
	AInteractableItem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerSphere;	// 触发的球碰撞体

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;	// 显示外观网格模型

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* IdleParticleComponent;	// 待交互时的粒子特效

	UPROPERTY(EditAnywhere, Category = "Interactable Item|Particle")
	UParticleSystem* OverlapParticle;	// 交互时的粒子特效

	UPROPERTY(EditAnywhere, Category = "Interactable Item|Sounds")
	USoundCue* OverlapSound;			// 交互时的声音特效

	UPROPERTY(EditAnywhere, Category = "Interactable Item|Properties")
	bool bNeedRotate = true;

	UPROPERTY(EditAnywhere, Category = "Interactable Item|Properties")
	float RotationRate = 45.0f;

protected:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
