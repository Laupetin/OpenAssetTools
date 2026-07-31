#include "RawLoaderPhysPresetT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/T4.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetT4.h"
#include "PhysPreset/PhysPresetCommon.h"
#include "Utils/Logging/Log.h"

using namespace T4;

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
            const auto fileName = phys_preset::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        phys_preset::InfoStringLoaderT4 m_info_string_loader;
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath, zone);
    }
} // namespace phys_preset
