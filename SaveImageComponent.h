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
	 * Writes pixels of render target texture as an image to disk.
	 */
	void SaveImage();

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
	 * Replacing RenderTarget::ReadPixels() because FlushRenderingCommands() has to be run
	 * in the game thread and blocks it, which causes FPS to drop.
	 * References:
	 * https://michaeljcole.github.io/wiki.unrealengine.com/Render_Target_Lookup/
	 * https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/UnrealClient.cpp#L49
	 */
	// bool ReadPixels(TArray<FColor>& OutImageData, FReadSurfaceDataFlags InFlags, FTextureRenderTargetResource* Resource);

	UPROPERTY(EditAnywhere) UTextureRenderTarget2D* TextureTarget;
	UPROPERTY(EditAnywhere) FString FilePath = "/home/swarm/unreal_ssd/Unreal_Saves/Images/";  // Absolute path to directoy where images should be saved.
	UPROPERTY(EditAnywhere) FString FileNamePrefix;  // Prefix of image file name. i.e. for a color image, "color_".
	UPROPERTY(EditAnywhere) int SavePerTick = 30;  // How often to save images. i.e. if SavePerTick = 3, images will be saved every 3 ticks.
	UPROPERTY(EditAnywhere) bool DisableSaving;  // If true, don't save images to disk.

private:
	int64 ticks_;  // Count of number of ticks that have passed since game started.
};


/**
 * Task to asynchronously save a PNG color image to disk such that ReadPixels doesn't block the game thread.
 * References:
 * https://github.com/unrealcv/unrealcv/blob/master/Source/UnrealCV/Private/GTCaptureComponent.cpp#L228
 * https://github.com/TimmHess/UnrealImageCapture/blob/master/CaptureToDisk/Source/CaptureToDisk/Private/CaptureManager.cpp#L225
 */
// class Image2Png2DiskAsyncTask : public FNonAbandonableTask {
// public:
// 	/**
// 	 * Constructs async task.
// 	 * @param SaveImageComp Entire instance to access capture component and render target.
// 	 * @param FileName Name to save image to.
// 	 * @param FilePath Path to directory of where image should be save.
// 	 */
// 	Image2Png2DiskAsyncTask(USaveImageComponent* SaveImageComp, const FString& FileName, const FString& FilePath = "/home/swarm/Downloads/Images/");
// 	~Image2Png2DiskAsyncTask(){}

// 	// Required by UE4!
//     FORCEINLINE TStatId GetStatId() const{
//         RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveImageToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
//     }

// 	// Executes save PNG color image to disk task.
// 	void DoWork();

// private:
// 	USaveImageComponent* SaveImageComp_;  // Used for access to the render target.
// 	FString EntireFilePath_;  // Path to save image including image name.
// };