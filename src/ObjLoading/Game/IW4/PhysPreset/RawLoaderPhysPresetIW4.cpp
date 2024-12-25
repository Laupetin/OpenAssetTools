#include "RawLoaderPhysPresetIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetIW4.h"

#include <format>
#include <iostream>

using namespace IW4;

RawLoaderPhysPreset::RawLoaderPhysPreset(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : m_memory(memory),
      m_search_path(searchPath),
      m_zone(zone)
{
}

AssetCreationResult RawLoaderPhysPreset::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    const auto fileName = std::format("physic/{}", assetName);
    const auto file = m_search_path.Open(fileName);
    if (!file.IsOpen())
        return AssetCreationResult::NoAction();

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
    {
        std::cerr << std::format("Could not parse as info string file: \"{}\"\n", fileName);
        return AssetCreationResult::Failure();
    }

    InfoStringLoaderPhysPreset infoStringLoader(m_memory, m_zone);
    return infoStringLoader.CreateAsset(assetName, infoString, context);
}
