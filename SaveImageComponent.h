// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Containers/UnrealString.h"
#include "Engine.h"  // Contains includes for almost all Unreal headers, i.e. USceneCaptureComponent2D.
#include "SaveImageComponent.generated.h"

/**
 * Utilities for SceneCaptureComponent2D to save color + depth images to disk.
 * Notes:
 *   - CaptureEveryFrame of USceneCaptureComponent has to be set to TRUE.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PED_API USaveImageComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties,
	USaveImageComponent();

protected:
	// Called when the game starts,
	virtual void BeginPlay() override;

public:	
	// Called every frame.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Converts raw image data to compressed PNG image data.
	 * @param Image  Raw image data.
	 * @param Width  Width of image. (i.e. number of rows)
	 * @param Height Height of image. (i.e. number of columns)
	 * References:
	 * https://github.com/unrealcv/unrealcv/blob/master/Source/UnrealCV/Private/Serialization.cpp#L67
	 */
	TArray<uint8> Image2Png(const TArray<FColor>& Image, int32 Width, int32 Height);

	/**
	 * Writes color and depth images to disk.
	 * FilePath: Absolute path to directory where images should be saved i.e. /home/swarm/Downloads/Images/
	 * ColorName: Prefix to name of color image. Suffix will tick number.
	 * DepthName: Prefix to name of depth image. Suffix will tick number.
	 */
	void SaveImage(const FString& FilePath = "/home/swarm/Downloads/Images/", const FString& ColorName = "color_", const FString& DepthName = "depth_");

	UPROPERTY(EditAnywhere) UTextureRenderTarget2D* TextureTarget;
	UPROPERTY(EditAnywhere) bool DisableSaving;  // If true, don't save images to disk.

private:
	int64 ticks_;  // Count of number of ticks that have passed since game started.
};
