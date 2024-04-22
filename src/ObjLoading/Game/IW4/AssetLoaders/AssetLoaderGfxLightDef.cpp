#include "AssetLoaderGfxLightDef.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>
#include <sstream>

using namespace IW4;

std::string AssetLoaderGfxLightDef::GetAssetFilename(const std::string& assetName)
{
    std::ostringstream ss;

    ss << "lights/" << assetName;

    return ss.str();
}

void* AssetLoaderGfxLightDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* lightDef = memory->Create<GfxLightDef>();
    memset(lightDef, 0, sizeof(GfxLightDef));
    lightDef->name = memory->Dup(assetName.c_str());
    return lightDef;
}

bool AssetLoaderGfxLightDef::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderGfxLightDef::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto filename = GetAssetFilename(assetName);
    const auto file = searchPath->Open(filename);
    if (!file.IsOpen())
        return false;

    const auto imageNameSize = file.m_length - sizeof(char) - sizeof(char);
    if (imageNameSize < 0 || imageNameSize > MAX_IMAGE_NAME_SIZE)
        return false;

    std::string imageName(static_cast<size_t>(imageNameSize), '\0');

    int8_t samplerState;
    int8_t lmapLookupStart;
    file.m_stream->read(reinterpret_cast<char*>(&samplerState), sizeof(int8_t));
    file.m_stream->read(&imageName[0], static_cast<size_t>(imageNameSize));
    file.m_stream->read(reinterpret_cast<char*>(&lmapLookupStart), sizeof(int8_t));

    auto* imageDependency = manager->LoadDependency<AssetImage>(imageName);

    if (!imageDependency)
    {
        std::cerr << "Could not load GfxLightDef \"" << assetName << "\" due to missing image \"" << imageName << "\"\n";
        return false;
    }

    auto* lightDef = memory->Create<GfxLightDef>();
    lightDef->name = memory->Dup(assetName.c_str());
    lightDef->attenuation.samplerState = samplerState;
    lightDef->attenuation.image = imageDependency->Asset();
    lightDef->lmapLookupStart = static_cast<int>(static_cast<uint8_t>(lmapLookupStart));

    manager->AddAsset(ASSET_TYPE_LIGHT_DEF, assetName, lightDef);

    return true;
}
