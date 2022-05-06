// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState:uint8
{
	EWS_Pickup		UMETA(DisplayName = "Pickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	EWS_MAX			UMETA(DisplayName = "DefaultMax")
};

/**
 *
 */
UCLASS()
class MYFIRSTPROJECT_API AWeapon: public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
		FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
		EWeaponState WeaponState;

	/// sound to play on equipping a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
		class USoundCue* OnEquipSound;

	/// sound to play when attacking with normal attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
		USoundCue* SwingSound;

	/// sound to play when attacking with strong attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
		USoundCue* SwingSoundTwo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
		bool bWeaponParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Combat")
		class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
		float Damage;

protected:
	virtual void BeginPlay() override;

public:

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMain* Char);

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; };

	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; };

	UFUNCTION()
		void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/// To call from animation blueprint to enable collision on specific AnimNotify
	UFUNCTION(BlueprintCallable)
		void ActivateCollision();

	/// To call from animation blueprint to disable collision on specific AnimNotify
	UFUNCTION(BlueprintCallable)
		void DeActivateCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		AController* WeaponInstegator;

	FORCEINLINE void SetInstegator(AController* Inst) { WeaponInstegator = Inst; };

	FORCEINLINE void SetNormalDamage(float NewDamage) { Damage = NewDamage; };
	FORCEINLINE void SetStrongDamage(float NewDamage) { Damage = NewDamage; };
};
