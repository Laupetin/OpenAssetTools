#include "AssetLoaderGfxImage.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "Image/IwiLoader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <zlib.h>

using namespace T6;

void* AssetLoaderGfxImage::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* image = memory->Create<GfxImage>();
    memset(image, 0, sizeof(GfxImage));
    image->name = memory->Dup(assetName.c_str());
    return image;
}

bool AssetLoaderGfxImage::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderGfxImage::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "images/" + assetName + ".iwi";
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    const auto fileSize = static_cast<size_t>(file.m_length);
    const auto fileData = std::make_unique<char[]>(fileSize);
    file.m_stream->read(fileData.get(), fileSize);
    const auto dataHash = static_cast<unsigned>(crc32(0u, reinterpret_cast<const Bytef*>(fileData.get()), fileSize));

    MemoryManager tempMemory;
    IwiLoader iwiLoader(&tempMemory);
    std::istringstream ss(std::string(fileData.get(), fileSize));
    const auto texture = iwiLoader.LoadIwi(ss);
    if (!texture)
    {
        std::cerr << "Failed to load texture from: " << fileName << "\n";
        return false;
    }

    auto* image = memory->Create<GfxImage>();
    memset(image, 0, sizeof(GfxImage));

    image->name = memory->Dup(assetName.c_str());
    image->hash = Common::R_HashString(image->name, 0);
    image->delayLoadPixels = true;

    image->noPicmip = !texture->HasMipMaps();
    image->width = static_cast<uint16_t>(texture->GetWidth());
    image->height = static_cast<uint16_t>(texture->GetHeight());
    image->depth = static_cast<uint16_t>(texture->GetDepth());

    image->streaming = 1;
    image->streamedParts[0].levelCount = 1;
    image->streamedParts[0].levelSize = static_cast<uint32_t>(fileSize);
    image->streamedParts[0].hash = dataHash & 0x1FFFFFFF;
    image->streamedPartCount = 1;

    manager->AddAsset(ASSET_TYPE_IMAGE, assetName, image);

    return true;
}
