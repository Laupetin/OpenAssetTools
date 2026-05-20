#include "RawLoaderWeaponIW6.h"

#include "Game/IW6/IW6.h"
#include "Game/IW6/ObjConstantsIW6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderWeaponIW6.h"
#include "Utils/Logging/Log.h"
#include "Weapon/WeaponCommon.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW6;
using namespace ::weapon;

namespace
{
    class RawLoaderWeapon final : public AssetCreator<AssetWeapon>
    {
    public:
        RawLoaderWeapon(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
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
            if (!infoString.FromStream(INFO_STRING_PREFIX_WEAPON, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        weapon::InfoStringLoaderIW6 m_info_string_loader;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoaderIW6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderWeapon>(memory, searchPath, zone);
    }
} // namespace weapon
