// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Main.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	/// Delegate functionality with events
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotate)
	{
		/// Rotate Items with specific rotation rate
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		SetActorRotation(Rotation);
	}
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/// Disable sound and particles if an item overlaps with anything other than a character (AMain)
	AMain* Main = Cast<AMain>(OtherActor);

	if (Main)
	{
		if (OverlapParticles)
		{
			/// Spawn particles on item location
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
		}

		if (OverlapSound)
		{
			/// Play sound before destroying the item
			UGameplayStatics::PlaySound2D(this, OverlapSound);
		}
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}