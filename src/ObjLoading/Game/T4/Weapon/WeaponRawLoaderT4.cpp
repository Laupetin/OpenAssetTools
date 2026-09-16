#include "WeaponRawLoaderT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/T4.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Weapon/WeaponCommon.h"
#include "WeaponInfoStringLoaderT4.h"

#include <array>
#include <format>

using namespace T4;

namespace
{
    SearchPathOpenFile OpenWeaponFile(ISearchPath& searchPath, const std::string& assetName, std::string& fileName)
    {
        const std::array candidates{
            weapon::GetFileNameForAssetName(assetName),
            std::format("weapons/sp/{}", assetName),
            std::format("weapons/mp/{}", assetName),
        };

        for (const auto& candidate : candidates)
        {
            auto file = searchPath.Open(candidate);
            if (file.IsOpen())
            {
                fileName = candidate;
                return file;
            }
        }

        // The game and its Windows filesystem treat archive paths
        // case-insensitively, while our IWD lookup preserves case.
        searchPath.Find(
            [&candidates, &fileName](const std::string& candidate)
            {
                if (!fileName.empty())
                    return;

                for (const auto& expected : candidates)
                {
                    if (utils::StringEqualsIgnoreCase(candidate, expected))
                    {
                        fileName = candidate;
                        return;
                    }
                }
            });

        return fileName.empty() ? SearchPathOpenFile() : searchPath.Open(fileName);
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
            auto file = OpenWeaponFile(m_search_path, assetName, fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_WEAPON, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            file.m_stream.reset();
            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        weapon::InfoStringLoaderT4 m_info_string_loader;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderWeapon>(memory, searchPath, zone);
    }
} // namespace weapon
