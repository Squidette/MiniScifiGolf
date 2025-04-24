// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolfBallBase.generated.h"

UCLASS()
class MINISCIFIGOLF_API AGolfBallBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGolfBallBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;
	
	UPROPERTY(EditAnywhere, Category = Projectile)
	FRotator HitDirection;

	UPROPERTY(EditAnywhere, Category = Projectile)
	float HitForce;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;
	
	UFUNCTION()
	void Shot();
};
