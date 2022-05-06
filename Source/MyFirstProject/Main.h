// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

/// Player Movement Status
UENUM(BlueprintType)
enum class EMovementStatus: uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/// Player Stamina Status
UENUM(BlueprintType)
enum class EStaminaStatus: uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MYFIRSTPROJECT_API AMain: public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
		TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
		class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
		class AItem* ActiveOverlappingItem;

	TArray<FVector> PickupLocations;

	/// Camea Boom for positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/// Follow Camera to follow the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/// Base Turn Rates to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animes")
		bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animes")
		bool bStrongAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animes")
		class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UParticleSystem* HitParticles;

	/// sound to play when hitting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class USoundCue* HitSound;

	///  ________________________________________________________ ///
	///  _____________________ player stats _____________________ ///
	///  ________________________________________________________ ///

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerStats")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
		float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerStats")
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
		float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
		int32 Coins;

	///  ________________________________________________________ ///
	///  _____________________ player Movement __________________ ///
	///  ________________________________________________________ ///

	/// Variable that contains Player Mpvement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EMovementStatus MovementStatus;

	/// Variable that contains Player Stamina Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EStaminaStatus StaminaStatus;

	/// Defining running speed to change animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float RunningSpeed;

	/// Defining sprinting speed to change animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float SprintingSpeed;

	/// bool to check if shift key is  pressed or released
	bool bShiftKeyDown;

	/// Rate to drain stamina in each frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float StaminaDrainRate;

	/// Specific portion of the stamina that is below the normal rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MinSprintStamina;

	/// speed of the interpolation (rotation) of the character towards the enemy attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float InterpSpeed;

	/// to check if the character is in the combat sphere of the enemy
	bool bInterpToEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		class AEnemy* CombatTarget;

	bool bMovingForward;
	bool bMovingRight;

	/// is left mouse button onhold
	bool bLMBDown;

	/// is right mouse button onhold
	bool bRMBDown;

	bool bESCDown;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Combat")
		bool bHasCombatTarget;

	UPROPERTY(visibleAnywhere, BlueprintReadWrite, Category = "Combat")
		FVector CombatTargetLocation;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Controller")
		class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<AEnemy> EnemyFilter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// Called for forward/backward input
	void MoveForward(float value);

	/// Called for left/right input
	void MoveRight(float value);

	/* Called via input to turn into a given rate
	* @Param Rate This is a normalized rate, i.e. 1.0 means 100% of the desired turn rate
	*/
	void TurnAtRate(float Rate);

	/* Called via input to look up/down at a given rate
	* @Param Rate This is a normalized rate, i.e. 1.0 means 100% of the desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	/// on press mouse left button
	void LMBDown();

	/// on release mouse left button
	void LMBUp();

	/// on press mouse right button
	void RMBDown();

	/// on release mouse right button
	void RMBUp();

	/// decrease health or take damage
	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/// increase coins count on coins picking
	UFUNCTION(BlueprintCallable)
		void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
		void IncrementHealth(float Amount);

	void Die();

	void UpdateCombatTarget();

	/// Set movement status and running speed
	void SetMovementStatus(EMovementStatus Status);

	/// press shift to start sprinting
	void ShiftKeyDown();

	/// release shift to start sprinting
	void ShiftKeyUp();

	/// Used to show debug sphere on pickup location after overlap begins
	UFUNCTION(BlueprintCallable)
		void ShowPickupLocations();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; } /// Change Stamina status depending on the stamina variable
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; } /// get the equipped weapon to destroy after picking another one
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; } /// save item to current overlapping item to set it on key press

	void SetEquippedWeapon(AWeapon* WeaponToSet); /// equip the weapon that we set from blueprints

	void Attack(); /// play attack animation,deal damage and disable player movement until finished
	void StrongAttack(); /// play strong /second attack animation,deal damage and disable player movement until finished

	UFUNCTION(BlueprintCallable)
		void AttackEnd(); /// stop attack animation and enable the player to start moving

	UFUNCTION(BlueprintCallable)
		void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
		void PlaySwingSoundTwo();

	/// change bInterpToEnemy to the opposite;
	void SetInterpToEnemy(bool Interp);

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; } /// set combat target (Enemy) to rotate

	FRotator GetLookAtRotationYaw(FVector Target);

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UFUNCTION(BlueprintCallable)
		void DeathEnd();

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
		void SaveGame();

	UFUNCTION(BlueprintCallable)
		void LoadGame(bool SetPosition);

	/// on press mouse left button
	void ESCDown();

	/// on release mouse left button
	void ESCUp();

	bool CanMove(float Value);

	void Turn(float Value);
	void LookUp(float Value);
};