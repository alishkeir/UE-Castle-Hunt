// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AFloatingPlatform: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/// Mesh for the floating platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
		class UStaticMeshComponent* Mesh;

	/// Starting point of the actor
	UPROPERTY(EditAnywhere)
		FVector StartPoint;

	/// Ending point of the actor
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
		FVector EndPoint;

	/// Speed of transition from StartPoint to EndPoint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
		float InterpSpeed;

	/// Time of transition between StartPoint and EndPoint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
		float InterpTime;

	/// Delay of transition between StartPoint and EndPoint
	UPROPERTY(EditAnywhere)
		FTimerHandle InterpTimer;

	/// Enable or disable transition between StartPoint and EndPoint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
		bool bInterping;

	/// To calculate distance between StartPoint and EndPoint
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// Enable/Disable Interping functionality
	void ToggleInterping();

	/// Swap two vectors ==> Basically it's used to change FloatingPlatform StartPoint and EndPoint
	void SwapVectors(FVector& Vec1, FVector& Vec2);
};
