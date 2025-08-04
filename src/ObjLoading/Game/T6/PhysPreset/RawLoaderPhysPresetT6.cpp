#include "RawLoaderPhysPresetT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetT6.h"
#include "PhysPreset/PhysPresetCommon.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;
using namespace ::phys_preset;

namespace
{
    class RawLoaderPhysPreset final : public AssetCreator<AssetPhysPreset>
    {
    public:
        RawLoaderPhysPreset(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
            {
                std::cerr << std::format("Could not parse as info string file: \"{}\"\n", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        T6::phys_preset::InfoStringLoader m_info_string_loader;
    };
} // namespace

namespace T6::phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath, zone);
    }
} // namespace T6::phys_preset
