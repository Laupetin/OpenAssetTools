#include "WeaponRawLoaderT4.h"

#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/T4.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Weapon/WeaponCommon.h"
#include "WeaponInfoStringLoaderT4.h"

#include <format>
#include <string_view>
#include <vector>

using namespace T4;

namespace
{
    bool HasWeaponDirectoryPrefix(const std::string_view assetName)
    {
        return assetName.size() > 3 && (assetName[2] == '/' || assetName[2] == '\\')
               && (utils::StringEqualsIgnoreCase(assetName.substr(0, 2), "sp") || utils::StringEqualsIgnoreCase(assetName.substr(0, 2), "mp"));
    }

    std::vector<std::string> GetWeaponFileCandidates(const std::string& assetName)
    {
        if (HasWeaponDirectoryPrefix(assetName))
            return {std::format("weapons/{}", assetName)};

        return {
            weapon::GetFileNameForAssetName(assetName),
            std::format("weapons/sp/{}", assetName),
            std::format("weapons/mp/{}", assetName),
        };
    }

    SearchPathOpenFile OpenWeaponFile(ISearchPath& searchPath, const std::vector<std::string>& candidates, std::string& fileName)
    {

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
        std::vector<std::string> caseInsensitiveMatches(candidates.size());
        searchPath.Find(
            [&candidates, &caseInsensitiveMatches](const std::string& candidate)
            {
                for (auto candidateIndex = 0u; candidateIndex < candidates.size(); candidateIndex++)
                {
                    if (caseInsensitiveMatches[candidateIndex].empty() && utils::StringEqualsIgnoreCase(candidate, candidates[candidateIndex]))
                        caseInsensitiveMatches[candidateIndex] = candidate;
                }
            });

        for (const auto& match : caseInsensitiveMatches)
        {
            if (!match.empty())
            {
                fileName = match;
                return searchPath.Open(fileName);
            }
        }

        return {};
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
            const auto candidates = GetWeaponFileCandidates(assetName);
            std::string fileName;
            auto file = OpenWeaponFile(m_search_path, candidates, fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_WEAPON, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            file.m_stream.reset();
            const auto actualAssetName = HasWeaponDirectoryPrefix(assetName) ? assetName.substr(3) : assetName;
            return m_info_string_loader.CreateAsset(actualAssetName, infoString, context);
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
