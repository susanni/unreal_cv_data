// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "Engine.h"  // USceneCaptureComponent2D
#include "GameFramework/Actor.h"
#include "SaveImageActor.generated.h"

// Utilities for SceneCaptureComponent2D to save color + depth images to disk.
UCLASS()
class PED_API ASaveImageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASaveImageActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Writes color and depth images to disk.
	 * FilePath: Absolute path to directory where images should be saved i.e. /home/swarm/Downloads/Images/
	 * ColorName: Prefix to name of color image. Suffix will tick number.
	 * DepthName: Prefix to name of depth image. Suffix will tick number.
	 */
	void SaveImage(const FString& FilePath = "/home/swarm/Downloads/Images/", const FString& ColorName = "color_", const FString& DepthName = "depth_");

    /**
	 * TODO(sni): Document.
	 */
	TArray<uint8> Image2Png(const TArray<FColor>& Image, int32 Width, int32 Height);

	UPROPERTY(EditAnywhere) USceneCaptureComponent2D* CaptureComponent;
	UPROPERTY(EditAnywhere) UTextureRenderTarget2D* TextureTarget;
	UPROPERTY(EditAnywhere) bool DisableSaving;  // If true, don't save images to disk.

private:
	int64 ticks;  // count of number of ticks that have passed since game started.
};
