// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Containers/UnrealString.h"
#include "Engine.h"  // Contains includes for almost all Unreal headers, i.e. USceneCaptureComponent2D.

#include "SavePoseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PED_API USavePoseComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties.
	USavePoseComponent();

protected:
	// Called when the game starts.
	virtual void BeginPlay() override;

public:	
	// Called every frame.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void WritePoseToFile();

	UPROPERTY(EditAnywhere) FString FilePath = "/home/swarm/Downloads/Unreal_Saves/Poses/";  // Absolute path to directoy where pose file is located/should be saved.
	UPROPERTY(EditAnywhere) FString FileName = "pose.txt";  // Name of pose file.
	UPROPERTY(EditAnywhere) int SavePerTick = 30;  // How often to save poses should be written. i.e. if SavePerTick = 3, poses will be written to the file on disk every 3 ticks.
	UPROPERTY(EditAnywhere) bool DisableSaving;  // If true, don't write poses to disk.

private:
	int64 ticks_;  // Count of number of ticks that have passed since game started.			
};
