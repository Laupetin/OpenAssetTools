#include "RawLoaderWeaponIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/ObjConstantsIW5.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderWeaponIW5.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

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
            const auto fileName = std::format("weapons/{}", assetName);
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
        InfoStringLoaderWeapon m_info_string_loader;
    };
} // namespace

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawWeaponLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderWeapon>(memory, searchPath, zone);
    }
} // namespace IW5
