// Copyright Keshav Prasad 2020

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetUpInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetGrabRange());
	}

	//if physics handle is attached
		//move object we are holding

	
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Log, TEXT("No Physics Handle component found on: %s"), *GetOwner()->GetName());
	}
}

void UGrabber::SetUpInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Null pointer reference on: %s"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	//try and reach any actors with physics body collision channel set		
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* PrimitiveToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocation
		(
			PrimitiveToGrab,
			NAME_None,
			GetGrabRange()
		);
	}
	
}

void UGrabber::Release()
{
	if (PhysicsHandle->GrabbedComponent)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->ReleaseComponent();
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType
	(
		OUT Hit,
		GetPlayerWorldPosition(),
		GetGrabRange(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams

	);

	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Error, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
	}

	return Hit;
}

FVector UGrabber::GetGrabRange() const
{
	FVector PlayerPosition;
	FRotator PlayerRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerPosition,
		OUT PlayerRotation
	);

	return PlayerPosition + PlayerRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayerWorldPosition() const
{
	FVector PlayerPosition;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerPosition,
		OUT PlayerRotation
	);

	return PlayerPosition;
}

