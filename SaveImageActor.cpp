// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveImageActor.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

// Sets default values.
ASaveImageActor::ASaveImageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned.
void ASaveImageActor::BeginPlay()
{
	Super::BeginPlay();

	if (DisableSaving) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("FYI: Image saving is disabled.")));
	}
}

// Called every frame.
void ASaveImageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	++ticks_;

	if (ticks_%30 == 0) {
		if (DisableSaving) return;
		SaveImage();
		// // Start an async task to save color image to disk so the game thread is not blocked (which drops the FPS).
		// (new FAutoDeleteAsyncTask<Image2Png2DiskAsyncTask>(this, "color_" + FString::FromInt(ticks_) + ".png"))->StartBackgroundTask();
	}
}

void ASaveImageActor::SaveImage(const FString& FilePath, const FString& ColorName, const FString& DepthName) {
	if (DisableSaving) return;

	int32 Width = TextureTarget->SizeX;
	int32 Height = TextureTarget->SizeY;

	// CaptureComponent->CaptureScene(); // Should not be used if bCaptureEveryFrame enabled.

	TArray<FColor> Image;
	Image.AddZeroed(Width * Height);
	
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(Image, ReadSurfaceDataFlags);

	TArray<uint8> ImgData = Image2Png(Image, Width, Height);
	FFileHelper::SaveArrayToFile(ImgData, *(FilePath + ColorName + FString::FromInt(ticks_) + ".png"));
}

TArray<uint8> ASaveImageActor::Image2Png(const TArray<FColor>& Image, int32 Width, int32 Height) {
	if (Image.Num() == 0 || Image.Num() != Width * Height) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Tick %d: Image.Num() == %d not equal Width * Height (%d * %d). Returning empty image."), ticks_, Image.Num(), Width, Height));
		return TArray<uint8>();
	}

	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	ImageWrapper->SetRaw(Image.GetData(), Image.Num()*sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8);
	TArray<uint8> ImgData;  // avoiding error: no known conversion from 'const TArray64<uint8>' (aka 'const TArray<unsigned char, TSizedDefaultAllocator<64> >') to 'std::initializer_list<unsigned char>' for 1st argument
	ImgData = ImageWrapper->GetCompressed();
	return ImgData;
}

// bool ASaveImageActor::ReadPixels(TArray<FColor>& OutImageData, FReadSurfaceDataFlags InFlags) {
// 	FTextureRenderTarget2DResource* RenderResource = (FTextureRenderTarget2DResource*) TextureTarget->Resource;
	
// 	// Read the render target surface data back.
// 	struct FReadSurfaceContext {
// 		FRenderTarget* SrcRenderTarget;
// 		TArray<FColor>* OutData;
// 		FIntRect Rect;
// 		FReadSurfaceDataFlags Flags;
// 	};

// 	OutImageData.Reset();

// 	FReadSurfaceContext Context =
// 	{
// 		RenderResource,
// 		&OutImageData,
// 		FIntRect(0, 0, RenderResource->GetSizeXY().X, RenderResource->GetSizeXY().Y),
// 		// InFlags
// 		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
// 	};

// 	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
// 		[Context](FRHICommandListImmediate& RHICmdList)
// 		{
// 			RHICmdList.ReadSurfaceData(
// 				Context.SrcRenderTarget->GetRenderTargetTexture(),
// 				Context.Rect,
// 				*Context.OutData,
// 				Context.Flags
// 			);
// 		});
// 	return OutImageData.Num() > 0;
// }


// Image2Png2DiskAsyncTask::Image2Png2DiskAsyncTask(ASaveImageActor* SaveImageActor, const FString& FileName, const FString& FilePath) {
// 	SaveImageActor_ = SaveImageActor;
// 	EntireFilePath_ = FilePath + FileName;
// }

// void Image2Png2DiskAsyncTask::DoWork() {
// 	int32 Width = SaveImageActor_->TextureTarget->SizeX;
// 	int32 Height = SaveImageActor_->TextureTarget->SizeY;

// 	// SaveImageActor_->CaptureComponent->CaptureScene();  // Should not be used if bCaptureEveryFrame enabled.

// 	TArray<FColor> Image;
// 	Image.AddZeroed(Width * Height);
	
// 	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("%s"), *EntireFilePath_));

// 	FReadSurfaceDataFlags ReadSurfaceDataFlags;
// 	ReadSurfaceDataFlags.SetLinearToGamma(false);

// 	if (SaveImageActor_->TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(Image, ReadSurfaceDataFlags)) {
// 		TArray<uint8> ImgData = SaveImageActor_->Image2Png(Image, Width, Height);
// 		FFileHelper::SaveArrayToFile(ImgData, *EntireFilePath_);
// 	} else {
// 		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("FAILED TO READ PIXELS ON TEXTURE TARGET.")));	
// 	}
// }