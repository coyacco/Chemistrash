// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedCameraActor.h"


// Sets default values
AFixedCameraActor::AFixedCameraActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFixedCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFixedCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

