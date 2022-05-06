// Fill out your copyright notice in the Description page of Project Settings.

#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// Create Camera Boom ( pulls towards the player if there's a collision )
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; /// Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; /// Rotate Arm based on  Controller

	/// Set Size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(53.0f, 106.0f);

	/// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; /// Disable Camera Rotation -- Will be Rotated with the CameraBoom orientation

	/// Set turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	/// Don't rotate character with controller rotating
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	/// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; /// character turns into the direction of the input ...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); /// ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.3;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150;
	Stamina = 120;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	InterpSpeed = 15.f;

	bInterpToEnemy = false;

	bShiftKeyDown = false;

	/// Initializing enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	bMovingForward = false;
	bMovingRight = false;
	bHasCombatTarget = false;

	bESCDown = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (this->GetVelocity().IsZero())
	{
		bMovingForward = false;
		bMovingRight = false;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime; /// Get stamina rate to drain each frame

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal: /// check if stamina status is normal
		if (bShiftKeyDown && !this->GetVelocity().IsZero())
		{
			if (Stamina - DeltaStamina <= MinSprintStamina) /// if the new stamina value is below the minimum value, set the status to below minimum and drain stamina
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			} else/// if not, just  drain stamina
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight) /// check if player is moving to add sprint ability
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			} else /// otherwise keep running only
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		} else /// Shift key up
		{
			if (Stamina + DeltaStamina >= MaxStamina) /// if new stamina value is above max, let stamina value equal to max
			{
				Stamina = MaxStamina;
			} else /// if not, just gain stamina
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal); /// set player movement status to normal
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum: /// check if stamina status is below normal
		if (bShiftKeyDown && !this->GetVelocity().IsZero())
		{
			if (Stamina - DeltaStamina <= 0.f) /// if new stamina is 0, keep the stamina to 0 and disable sprinting (se movement status to normal)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			} else /// if not, keep draining stamina
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)  /// check if player is moving to add spring ability
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				} else /// otherwise keep running only
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		} else /// Shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina) /// check if new stamina value is more than the minimium sprint allowed value and set it's status to normal
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			} else /// otherwise just keep gaining stamina
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:  /// check if stamina status is at exhausted and set stamina to 0
		if (bShiftKeyDown && !this->GetVelocity().IsZero())
		{
			Stamina = 0.f;
		} else /// Shift key up /// if not keep gaining stamina
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:  /// check if stamina status is recovering from exhausted
		if (Stamina + DeltaStamina >= MinSprintStamina) /// check if new stamina value is more than the minimium sprint allowed value and set it's status to normal
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		} else /// if not, just keep gaining stamina
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMain::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMain::RMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);
}

void AMain::MoveForward(float value)
{
	if (CanMove(value)) /// disable movement on attacking
	{
		/// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float value)
{
	if (CanMove(value))
	{
		/// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
		bMovingRight = true;
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMain::TurnAtRate(float Rate) /// turn camera left and right depending on a specific rate
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate) /// turn camera up and down  depending on a specific rate
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::DecrementHealth(float Amount)
{
	Health -= Amount;

	if (Health - Amount <= 0.f) /// if new decreased health is less than 0 the player should die
	{
		Health -= Amount;
		Die();
	} else
	{
		Health -= Amount; /// if not, just decrease health // take damage
	}
}

void AMain::LMBDown()
{
	bLMBDown = true;
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;

		if (ActiveOverlappingItem)
		{
			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);

			if (Weapon)
			{
				Weapon->Equip(this);
				SetActiveOverlappingItem(nullptr); /// set overlapping item to null after picking it
			}
		} else if (EquippedWeapon)
		{
			Attack();
		}
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::RMBDown()
{
	bRMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible) return;

		if (ActiveOverlappingItem)
		{
			return;
		} else if (EquippedWeapon)
		{
			StrongAttack();
		}
	}
}

void AMain::RMBUp()
{
	bRMBDown = false;
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount; /// increanse coins on catch
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	} else
	{
		Health += Amount;
	}
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	SetMovementStatus(EMovementStatus::EMS_Dead);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); /// get AnimeInstance from the mesh

	if (AnimInstance && CombatMontage) /// check if anime instance is valid, play the selected montage and jump to Attack_1 section
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	if (MovementStatus == EMovementStatus::EMS_Sprinting) /// check if player is sprinting
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed; /// change character speed to new sprinting speed
	} else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;  /// if not, change character speed to default running speed
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

/// loop on each pickup location and save it to the TArray
void AMain::ShowPickupLocations()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Yellow, 10.f, 0.7f); /// Debug sphere to show location of something on start
	}
}

/// equip the weapon that we set from blueprints
void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	/// destory the previous weapon from the world before picking the other

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	/// __________________________________ /////

	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (!bAttacking && !bStrongAttacking && MovementStatus != EMovementStatus::EMS_Dead) /// don't keep attackinh unil the ingoing attack finishes
	{
		bAttacking = true;

		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); /// get AnimeInstance from the mesh

		if (AnimInstance && CombatMontage) /// check if anime instance is valid, play the selected montage and jump to Attack_1 section
		{
			AnimInstance->Montage_Play(CombatMontage, 1.85f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"));
		}
	}
}

void AMain::StrongAttack()
{
	if (!bStrongAttacking && !bAttacking && MovementStatus != EMovementStatus::EMS_Dead) /// don't keep attackinh unil the ingoing attack finishes
	{
		bStrongAttacking = true;

		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); /// get AnimeInstance from the mesh

		if (AnimInstance && CombatMontage) /// check if anime instance is validd, play the selected montage and jump to Attack_1 section
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"));
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	bStrongAttacking = false;
	SetInterpToEnemy(false);

	if (bLMBDown) /// if user keeps hloding the LMB, keep attacking // keep repeating attack
	{
		Attack();
	} else if (bRMBDown)
	{
		StrongAttack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::PlaySwingSoundTwo()
{
	if (EquippedWeapon->SwingSoundTwo)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSoundTwo);
	}
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f) /// if new decreased health is less than 0 the player should die
	{
		Health = 0.f;

		Die();

		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	} else
	{
		Health -= DamageAmount; /// if not, just decrease health // take damage
	}

	return DamageAmount;
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy)
	{
		float minDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				if (DistanceToActor < minDistance)
				{
					ClosestEnemy = Enemy;
					minDistance = DistanceToActor;
				}
			}
		}

		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast< UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (MainPlayerController)
	{
		MainPlayerController->bPauseMenuVisible = false;
	}

	bAttacking = false;
	bStrongAttacking = false;

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}

	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	SetMovementStatus(EMovementStatus::EMS_Normal);
}

void AMain::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f && !bAttacking && !bStrongAttacking && MovementStatus != EMovementStatus::EMS_Dead && !MainPlayerController->bPauseMenuVisible);
	}
	return false;
}

void AMain::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMain::LookUp(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}