// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "Engine.h"  // Contains includes for almost all Unreal headers, i.e. USceneCaptureComponent2D.
#include "GameFramework/Actor.h"
#include "SaveImageActor.generated.h"

// Utilities for SceneCaptureComponent2D to save color + depth images to disk.
UCLASS()
class PED_API ASaveImageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties.
	ASaveImageActor();

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

public:	
	// Called every frame.
	virtual void Tick(float DeltaTime) override;

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

	/**
	 * Replacing RenderTarget::ReadPixels() because FlushRenderingCommands() has to be run
	 * in the game thread and blocks it, which causes FPS to drop.
	 * References:
	 * https://michaeljcole.github.io/wiki.unrealengine.com/Render_Target_Lookup/
	 * https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/UnrealClient.cpp#L49
	 */
	// bool ReadPixels(TArray<FColor>& OutImageData, FReadSurfaceDataFlags InFlags);

	UPROPERTY(EditAnywhere) USceneCaptureComponent2D* CaptureComponent;
	UPROPERTY(EditAnywhere) UTextureRenderTarget2D* TextureTarget;
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
class Image2Png2DiskAsyncTask : public FNonAbandonableTask {
public:
	/**
	 * Constructs async task.
	 * @param SaveImageActor Entire instance to access capture component and render target.
	 * @param FileName Name to save image to.
	 * @param FilePath Path to directory of where image should be save.
	 */
	Image2Png2DiskAsyncTask(ASaveImageActor* SaveImageActor, const FString& FileName, const FString& FilePath = "/home/swarm/Downloads/Images/");
	~Image2Png2DiskAsyncTask(){}

	// Required by UE4!
    FORCEINLINE TStatId GetStatId() const{
        RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveImageToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
    }

	// Executes save PNG color image to disk task.
	void DoWork();

private:
	ASaveImageActor* SaveImageActor_;  // Used to access capture component and render target.
	FString EntireFilePath_;  // Path to save image including image name.
};