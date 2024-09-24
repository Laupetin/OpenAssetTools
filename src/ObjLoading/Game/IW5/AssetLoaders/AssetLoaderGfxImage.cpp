#include "AssetLoaderGfxImage.h"

#include "Game/IW5/IW5.h"
#include "Image/IwiLoader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>

using namespace IW5;

void* AssetLoaderGfxImage::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetImage::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderGfxImage::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderGfxImage::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = std::format("images/{}.iwi", assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    const auto fileSize = static_cast<size_t>(file.m_length);
    const auto fileData = std::make_unique<char[]>(fileSize);
    file.m_stream->read(fileData.get(), fileSize);

    std::istringstream ss(std::string(fileData.get(), fileSize));
    const auto texture = iwi::LoadIwi(ss);
    if (!texture)
    {
        std::cerr << std::format("Failed to load texture from: {}\n", fileName);
        return false;
    }

    auto* image = memory->Create<GfxImage>();
    memset(image, 0, sizeof(GfxImage));

    image->name = memory->Dup(assetName.c_str());
    image->noPicmip = !texture->HasMipMaps();
    image->width = static_cast<uint16_t>(texture->GetWidth());
    image->height = static_cast<uint16_t>(texture->GetHeight());
    image->depth = static_cast<uint16_t>(texture->GetDepth());

    image->texture.loadDef = memory->Alloc<GfxImageLoadDef>();

    manager->AddAsset<AssetImage>(assetName, image);

    return true;
}
