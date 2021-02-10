// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePoseComponent.h"


// Sets default values for this component's properties.
USavePoseComponent::USavePoseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts.
void USavePoseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USavePoseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	++ticks_;

	if (DisableSaving) return;

	if (ticks_%SavePerTick == 0) {
		WritePoseToFile();
	}
}

void USavePoseComponent::WritePoseToFile() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("[SavePoseComponent] Pose: %s"), *(this->GetComponentTransform().ToHumanReadableString())));
}

