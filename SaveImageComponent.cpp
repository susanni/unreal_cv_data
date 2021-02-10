// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveImageComponent.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RHICommandList.h"

// Sets default values for this component's properties.
USaveImageComponent::USaveImageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts.
void USaveImageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (TextureTarget == nullptr) {
		DisableSaving = true;
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("[SaveImageComponent] No target texture provided. Disabling saving.")));
	}
	if (DisableSaving) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("[SaveImageComponent] FYI: Image saving is disabled.")));
	}
}


// Called every frame.
void USaveImageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	++ticks_;

	if (DisableSaving) return;

	if (ticks_%SavePerTick == 0) {
		SaveImage();
		// Start an async task to save color image to disk so the game thread is not blocked (which drops the FPS).
		// (new FAutoDeleteAsyncTask<Image2Png2DiskAsyncTask>(this, "color_" + FString::FromInt(ticks_) + ".png"))->StartBackgroundTask();
	}
}

void USaveImageComponent::SaveImage() {
	int32 Width = TextureTarget->SizeX;
	int32 Height = TextureTarget->SizeY;

	TArray<FColor> Image;
	Image.AddZeroed(Width * Height);
	
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(Image, ReadSurfaceDataFlags);

	TArray<uint8> ImgData = Image2Png(Image, Width, Height);
	FFileHelper::SaveArrayToFile(ImgData, *(FilePath + FileNamePrefix + FString::FromInt(ticks_) + ".png"));
}

TArray<uint8> USaveImageComponent::Image2Png(const TArray<FColor>& Image, int32 Width, int32 Height) {
	if (Image.Num() == 0 || Image.Num() != Width * Height) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("[SaveImageComponent] Tick %d: Image.Num() == %d not equal Width * Height (%d * %d). Returning empty image."), ticks_, Image.Num(), Width, Height));
		return TArray<uint8>();
	}

	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	ImageWrapper->SetRaw(Image.GetData(), Image.Num()*sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8);
	TArray<uint8> ImgData;  // avoiding error: no known conversion from 'const TArray64<uint8>' (aka 'const TArray<unsigned char, TSizedDefaultAllocator<64> >') to 'std::initializer_list<unsigned char>' for 1st argument
	ImgData = ImageWrapper->GetCompressed();
	return ImgData;
}

// bool USaveImageComponent::ReadPixels(TArray<FColor>& OutImageData, FReadSurfaceDataFlags InFlags, FTextureRenderTargetResource* Resource) {
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
// 		Resource,
// 		&OutImageData,
// 		// FIntRect(0, 0, RenderResource->GetSizeXY().X, RenderResource->GetSizeXY().Y),
// 		FIntRect(0, 0, TextureTarget->SizeX, TextureTarget->SizeY),
// 		// InFlags
// 		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
// 	};

// 	// ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
// 	// 	[Context](FRHICommandListImmediate& RHICmdList)
// 	// 	{
// 	// 		RHICmdList.ReadSurfaceData(
// 	// 			Context.SrcRenderTarget->GetRenderTargetTexture(),
// 	// 			Context.Rect,
// 	// 			*Context.OutData,
// 	// 			Context.Flags
// 	// 		);
// 	// 	});
// 	return OutImageData.Num() > 0;
// }


// Image2Png2DiskAsyncTask::Image2Png2DiskAsyncTask(USaveImageComponent* SaveImageComp, const FString& FileName, const FString& FilePath) {
// 	SaveImageComp_ = SaveImageComp;
// 	EntireFilePath_ = FilePath + FileName;
// }

// void Image2Png2DiskAsyncTask::DoWork() {
// 	int32 Width = SaveImageComp_->TextureTarget->SizeX;
// 	int32 Height = SaveImageComp_->TextureTarget->SizeY;

// 	TArray<FColor> Image;
// 	Image.AddZeroed(Width * Height);
	
// 	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("%s"), *EntireFilePath_));

// 	FReadSurfaceDataFlags ReadSurfaceDataFlags;
// 	ReadSurfaceDataFlags.SetLinearToGamma(false);

// 	if (SaveImageComp_->ReadPixels(Image, ReadSurfaceDataFlags, SaveImageComp_->TextureTarget->GameThread_GetRenderTargetResource())) {
// 		TArray<uint8> ImgData = SaveImageComp_->Image2Png(Image, Width, Height);
// 		FFileHelper::SaveArrayToFile(ImgData, *EntireFilePath_);
// 	} else {
// 		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("FAILED TO READ PIXELS ON TEXTURE TARGET.")));	
// 	}
// }