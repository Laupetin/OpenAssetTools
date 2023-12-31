#include "LocalizeCommonAssetLoader.h"

#include "Localize/LocalizeCommon.h"
#include "Localize/LocalizeReadingZoneState.h"
#include "Localize/Parsing/LocalizeFileReader.h"

#include <sstream>

LocalizeCommonAssetLoader::LocalizeCommonAssetLoader(std::function<void(const CommonLocalizeEntry&)> entryCallback)
    : m_entry_callback(std::move(entryCallback))
{
}

std::string LocalizeCommonAssetLoader::GetFileName(const std::string& assetName, Zone* zone) const
{
    std::ostringstream ss;
    ss << LocalizeCommon::GetNameOfLanguage(zone->m_language) << "/localizedstrings/" << assetName << ".str";
    return ss.str();
}

bool LocalizeCommonAssetLoader::LoadLocalizeAsset(const std::string& assetName, ISearchPath* searchPath, IAssetLoadingManager* manager, Zone* zone) const
{
    std::string fileName = GetFileName(assetName, zone);

    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    auto* zoneState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<LocalizeReadingZoneState>();
    LocalizeFileReader reader(*file.m_stream, assetName, zone->m_language, zoneState);

    std::vector<CommonLocalizeEntry> localizeEntries;
    if (!reader.ReadLocalizeFile(localizeEntries))
        return false;

    for (const auto& entry : localizeEntries)
    {
        m_entry_callback(entry);
    }

    return true;
}
