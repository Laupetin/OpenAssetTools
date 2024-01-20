#include "AssetLoaderLocalizeEntry.h"

#include "Localize/LocalizeCommonAssetLoader.h"

using namespace IW4;

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
    const LocalizeCommonAssetLoader commonLoader(
        [memory, manager](const CommonLocalizeEntry& entry)
        {
            auto* localizeEntry = memory->Create<LocalizeEntry>();
            localizeEntry->name = memory->Dup(entry.m_key.c_str());
            localizeEntry->value = memory->Dup(entry.m_value.c_str());

            manager->AddAsset(ASSET_TYPE_LOCALIZE_ENTRY, entry.m_key, localizeEntry);
        });

    return commonLoader.LoadLocalizeAsset(assetName, searchPath, manager, zone);
}
