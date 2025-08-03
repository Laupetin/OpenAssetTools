#include "WeaponRawLoaderT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "Weapon/WeaponCommon.h"
#include "WeaponInfoStringLoaderT6.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;
using namespace ::weapon;

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
            const auto fileName = GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON, *file.m_stream))
            {
                std::cerr << std::format("Could not parse as info string file: \"{}\"\n", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        T6::weapon::InfoStringLoader m_info_string_loader;
    };
} // namespace

namespace T6::weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoader>(memory, searchPath, zone);
    }
} // namespace T6::weapon
