// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AFloorSwitch: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorSwitch();

	/// Overlap volume for functionality to be triggered
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		class UBoxComponent* TriggerBox;

	/// Switch for the character to step on
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		class UStaticMeshComponent* FloorSwitch;

	/// door to move when the FloorSwitch is stepped on
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		UStaticMeshComponent* Door;

	/// Initial location for the door
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
		FVector InitialDoorLocation;

	/// Initial location for the FloorSwitch
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
		FVector InitialSwitchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
		FTimerHandle SwitchHandle;

	float SwitchTime;

	bool bCharacterOnSwitch;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// To be called when the player steps on the FloorSwitch
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// To be called when the player steps out of the FloorSwitch
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/// This is an event that doesn't need implementation, it will be assigned to dosomething as it is called /// Will be delegated to OnOverlapBegin for opening the door
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void RaiseDoor();

	///  Will be delegated to OnOverlapEnd for closing the door
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void LowerDoor();

	/// Will be delegated to OnOverlapEnd to return FloorSwitch to it's original location
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void RaiseFloorSwitch();

	/// Will be delegated to OnOverlapBegin to return translate the FloorSwitch down in the Z-Direction
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
		void LowerFloorSwitch();

	/// Set door location to it's new origin
	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
		void UpdateDoorLocation(float Z);

	/// Set door FloorSwitch to it's new origin
	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
		void UpdateFloorSwitchLocation(float Z);
};
