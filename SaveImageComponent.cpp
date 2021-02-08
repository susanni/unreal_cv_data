// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveImageComponent.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

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
	if (DisableSaving) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("[SaveImageComponent] FYI: Image saving is disabled.")));
	}
}


// Called every frame.
void USaveImageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	++ticks_;

	if (ticks_%30 == 0) {
		if (DisableSaving) return;
		SaveImage();
	}
}

void USaveImageComponent::SaveImage(const FString& FilePath, const FString& ColorName, const FString& DepthName) {
	if (DisableSaving) return;

	int32 Width = TextureTarget->SizeX;
	int32 Height = TextureTarget->SizeY;

	TArray<FColor> Image;
	Image.AddZeroed(Width * Height);
	
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(Image, ReadSurfaceDataFlags);

	TArray<uint8> ImgData = Image2Png(Image, Width, Height);
	FFileHelper::SaveArrayToFile(ImgData, *(FilePath + ColorName + FString::FromInt(ticks_) + ".png"));
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