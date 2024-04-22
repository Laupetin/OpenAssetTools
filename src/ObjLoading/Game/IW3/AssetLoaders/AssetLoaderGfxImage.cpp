#include "AssetLoaderGfxImage.h"

#include "Game/IW3/IW3.h"
#include "Image/DdsLoader.h"
#include "Image/IwiTypes.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>

using namespace IW3;

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
    // Do not load any GfxImages from raw for now that are not loaded
    // TODO: Load iwis and add streaming info to asset
    if (assetName.empty() || assetName[0] != '*')
        return false;

    std::string safeAssetName = assetName;
    for (auto& c : safeAssetName)
    {
        switch (c)
        {
        case '*':
            c = '_';
            break;

        default:
            break;
        }
    }

    const auto file = searchPath->Open("images/" + safeAssetName + ".dds");
    if (!file.IsOpen())
        return false;

    const DdsLoader ddsLoader(zone->GetMemory());
    auto* texture = ddsLoader.LoadDds(*file.m_stream);

    if (texture == nullptr)
    {
        std::cout << "Failed to load dds file for image asset \"" << assetName << "\"\n";
        return false;
    }

    auto* image = memory->Create<GfxImage>();
    image->name = memory->Dup(assetName.c_str());
    image->picmip.platform[0] = 0;
    image->picmip.platform[1] = 0;
    image->noPicmip = !texture->HasMipMaps();
    image->semantic = TS_FUNCTION;
    image->track = 0;
    image->cardMemory.platform[0] = 0;
    image->cardMemory.platform[1] = 0;
    image->width = static_cast<uint16_t>(texture->GetWidth());
    image->height = static_cast<uint16_t>(texture->GetHeight());
    image->depth = static_cast<uint16_t>(texture->GetDepth());
    image->category = IMG_CATEGORY_AUTO_GENERATED;
    image->delayLoadPixels = false;

    switch (texture->GetTextureType())
    {
    case TextureType::T_2D:
        image->mapType = MAPTYPE_2D;
        break;

    case TextureType::T_3D:
        image->mapType = MAPTYPE_3D;
        break;

    case TextureType::T_CUBE:
        image->mapType = MAPTYPE_CUBE;
        break;

    default:
        image->mapType = MAPTYPE_NONE;
        break;
    }

    const auto mipCount = texture->HasMipMaps() ? texture->GetMipMapCount() : 1;
    const auto faceCount = texture->GetFaceCount();

    size_t dataSize = 0;
    for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
        dataSize += texture->GetSizeOfMipLevel(mipLevel) * faceCount;

    auto* loadDef = static_cast<GfxImageLoadDef*>(zone->GetMemory()->AllocRaw(offsetof(GfxImageLoadDef, data) + dataSize));
    image->texture.loadDef = loadDef;
    loadDef->levelCount = static_cast<char>(mipCount);
    loadDef->flags = 0;
    if (!texture->HasMipMaps())
        loadDef->flags |= iwi6::IMG_FLAG_NOMIPMAPS;
    if (texture->GetTextureType() == TextureType::T_CUBE)
        loadDef->flags |= iwi6::IMG_FLAG_CUBEMAP;
    if (texture->GetTextureType() == TextureType::T_3D)
        loadDef->flags |= iwi6::IMG_FLAG_VOLMAP;
    loadDef->dimensions[0] = image->width;
    loadDef->dimensions[1] = image->height;
    loadDef->dimensions[2] = image->depth;
    loadDef->format = static_cast<int>(texture->GetFormat()->GetD3DFormat());
    loadDef->resourceSize = dataSize;

    char* currentDataBuffer = loadDef->data;
    for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
    {
        const auto mipSize = texture->GetSizeOfMipLevel(mipLevel);

        for (auto face = 0; face < faceCount; face++)
        {
            memcpy(currentDataBuffer, texture->GetBufferForMipLevel(mipLevel, face), mipSize);
            currentDataBuffer += mipSize;
        }
    }

    manager->AddAsset(ASSET_TYPE_IMAGE, assetName, image);

    return true;
}
