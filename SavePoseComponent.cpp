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
	if (!DisableSaving) {
		if (FileName.IsEmpty()) {
			FileName = this->GetAttachmentRootActor()->GetName() + ".txt";
		}
		FString Heading = "Tick,POS_X,POS_Y,POS_Z,ROT_X,ROT_Y,ROT_Z,ROT_W\n";
		FFileHelper::SaveStringToFile(*Heading, *(FilePath+FileName), FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);
	}
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
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("[SavePoseComponent] Pose: %s"), *(this->GetComponentTransform().ToHumanReadableString())));
	FTransform Transform = this->GetComponentTransform();
	FVector Trans = Transform.GetTranslation();
	FQuat Rot = Transform.GetRotation();
	FString Pose = FString::FromInt(ticks_) + "," + FString::SanitizeFloat(Trans.X) + "," + FString::SanitizeFloat(Trans.Y) + "," + FString::SanitizeFloat(Trans.Z)
	             + "," + FString::SanitizeFloat(Rot.X) + "," + FString::SanitizeFloat(Rot.Y) + "," + FString::SanitizeFloat(Rot.Z) + "," + FString::SanitizeFloat(Rot.W) + "\n";
	FFileHelper::SaveStringToFile(*Pose, *(FilePath+FileName), FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}