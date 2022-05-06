// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Main.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
	bWeaponParticles = false;
	WeaponState = EWeaponState::EWS_Pickup;

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); /// to do not generate overlap events
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); /// automatic overlap parameters set
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); /// ignore all channels collision by defaults
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); /// set collision to overlap for for pawn
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor) /// check if the item is valid and save it into the overlapping variable
	{
		AMain* Main = Cast<AMain>(OtherActor);

		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor) /// check if the item is valid and save it into the overlapping variable
	{
		AMain* Main = Cast<AMain>(OtherActor);

		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr); /// remove the item from the variable on leaving the sphere
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if (Char)
	{
		SetInstegator(Char->GetController());

		/// do not let the camera zoom if the sword is between the camera and the player
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

		/// disable collision between sword and pawn to let it keep moving
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		/// disable physics simulatin to attach the weapon to the player
		SkeletalMesh->SetSimulatePhysics(false);

		/// Get socket by name from engine and attach the USkeletalMeshSocket to it
		const USkeletalMeshSocket* RightHandsocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		/// check if the socket is valid and attach the skeletal mesh to it
		if (RightHandsocket)
		{
			RightHandsocket->AttachActor(this, Char->GetMesh());
			bRotate = false;

			Char->SetEquippedWeapon(this); /// set the new weapon to the selected one
			Char->SetActiveOverlappingItem(nullptr); /// remove the item from the variable on leaving the sphere
			WeaponState = EWeaponState::EWS_Equipped; /// set status to equipped to disable overlapping again /// every Weapon_BP has it's own WeaponState, so it won't bug
		}

		if (OnEquipSound) /// check if equip sound is set to a weapon and play it when player equip it
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}

		if (!bWeaponParticles) /// deactivate weapon particles upon equiping /// can be set from the editor to keep particles or no
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				/// Spawn particles on the socket attached to weapon when hitting an enemy
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
				/// play hit sound when sword hit the enemy capsule
				if (Enemy->HitSound)
				{
					UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
				}
				if (DamageTypeClass)
				{
					UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstegator, this, DamageTypeClass);
				}
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); ///  generate overlap events to specific target
}

void AWeapon::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); /// to do not generate overlap events
}