#include "AssetLoaderLocalizeEntry.h"

#include "Localize/LocalizeCommon.h"
#include "Parsing/LocalizeFile/LocalizeFileReader.h"

#include <sstream>

using namespace IW5;

XAssetInfoGeneric* AssetLoaderLocalizeEntry::LoadFromGlobalAssetPools(const std::string& assetName) const
{
    return nullptr;
}

void* AssetLoaderLocalizeEntry::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    return nullptr;
}

bool AssetLoaderLocalizeEntry::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderLocalizeEntry::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    std::string fileName;
    {
        std::ostringstream str;
        str << LocalizeCommon::GetNameOfLanguage(zone->m_language) << "/localizedstrings/" << assetName << ".str";
        fileName = str.str();
    }

    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    LocalizeFileReader reader(*file.m_stream, assetName, zone->m_language);
    const auto localizeEntries = reader.ReadLocalizeFile();

    for (const auto& entry : localizeEntries)
    {
        auto* localizeEntry = memory->Create<LocalizeEntry>();
        localizeEntry->name = memory->Dup(entry.m_key.c_str());
        localizeEntry->value = memory->Dup(entry.m_value.c_str());

        manager->AddAsset(ASSET_TYPE_LOCALIZE_ENTRY, entry.m_key, localizeEntry);
    }

    return true;
}
