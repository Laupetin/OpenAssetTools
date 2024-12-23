#include "CommonLocalizeLoader.h"

#include "Localize/LocalizeCommon.h"
#include "Localize/Parsing/LocalizeFileReader.h"

#include <format>

CommonLocalizeLoader::CommonLocalizeLoader(ISearchPath& searchPath, Zone& zone)
    : m_search_path(searchPath),
      m_zone(zone)
{
}

std::string CommonLocalizeLoader::GetFileName(const std::string& assetName) const
{
    return std::format("{}/localizedstrings/{}.str", LocalizeCommon::GetNameOfLanguage(m_zone.m_language), assetName);
}

AssetCreationResult CommonLocalizeLoader::CreateLocalizeAsset(const std::string& assetName, AssetCreationContext& context)
{
    std::string fileName = GetFileName(assetName);

    const auto file = m_search_path.Open(fileName);
    if (!file.IsOpen())
        return AssetCreationResult::NoAction();

    LocalizeFileReader reader(*file.m_stream, assetName, m_zone.m_language, *this);

    std::vector<CommonLocalizeEntry> localizeEntries;
    if (!reader.ReadLocalizeFile(localizeEntries))
        return AssetCreationResult::Failure();

    auto lastResult = AssetCreationResult::Failure();
    for (const auto& entry : localizeEntries)
    {
        lastResult = CreateAssetFromCommonAsset(entry, context);
        if (!lastResult.HasBeenSuccessful())
            return lastResult;
    }

    return lastResult;
}

bool CommonLocalizeLoader::CheckLocalizeEntryForDuplicates(const std::string& key)
{
    const auto existingEntry = m_keys.find(key);
    if (existingEntry != m_keys.end())
        return false;

    m_keys.emplace(key);
    return true;
}
