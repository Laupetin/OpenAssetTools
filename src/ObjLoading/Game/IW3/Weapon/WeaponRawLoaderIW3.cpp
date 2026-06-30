#include "WeaponRawLoaderIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/ObjConstantsIW3.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"
#include "Weapon/WeaponCommon.h"
#include "WeaponInfoStringLoaderIW3.h"

using namespace IW3;

namespace
{
    SearchPathOpenFile OpenWeaponFile(ISearchPath& searchPath, const std::string& assetName, std::string& fileName)
    {
        fileName = weapon::GetFileNameForAssetName(assetName);
        return searchPath.Open(fileName);
    }

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
            std::string fileName;
            const auto file = OpenWeaponFile(m_search_path, assetName, fileName);
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
        weapon::InfoStringLoaderIW3 m_info_string_loader;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderWeapon>(memory, searchPath, zone);
    }
} // namespace weapon
