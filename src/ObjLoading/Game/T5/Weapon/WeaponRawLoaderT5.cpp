#include "WeaponRawLoaderT5.h"

#include "Game/T5/ObjConstantsT5.h"
#include "Game/T5/T5.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"
#include "Weapon/WeaponCommon.h"
#include "WeaponInfoStringLoaderT5.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T5;

namespace
{
    class RawLoader final : public AssetCreator<AssetWeapon>
    {
    public:
        RawLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = weapon::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_WEAPON, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        weapon::InfoStringLoaderT5 m_info_string_loader;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoader>(memory, searchPath, zone);
    }
} // namespace weapon
