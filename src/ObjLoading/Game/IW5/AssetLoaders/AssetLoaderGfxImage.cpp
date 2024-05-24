#include "AssetLoaderGfxImage.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderGfxImage::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* image = memory->Create<GfxImage>();
    memset(image, 0, sizeof(GfxImage));
    image->name = memory->Dup(assetName.c_str());
    return image;
}
