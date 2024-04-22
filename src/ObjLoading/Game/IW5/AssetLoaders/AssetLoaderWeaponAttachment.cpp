#include "AssetLoaderWeaponAttachment.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/Weapon/JsonWeaponAttachmentLoader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

void* AssetLoaderWeaponAttachment::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* attachment = memory->Create<WeaponAttachment>();
    memset(attachment, 0, sizeof(WeaponAttachment));
    attachment->szInternalName = memory->Dup(assetName.c_str());

    return attachment;
}

bool AssetLoaderWeaponAttachment::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderWeaponAttachment::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(std::format("attachment/{}.json", assetName));
    if (!file.IsOpen())
        return false;

    auto* attachment = memory->Alloc<WeaponAttachment>();
    attachment->szInternalName = memory->Dup(assetName.c_str());

    std::vector<XAssetInfoGeneric*> dependencies;
    std::vector<IndirectAssetReference> indirectAssetReferences;
    if (LoadWeaponAttachmentAsJson(*file.m_stream, *attachment, memory, manager, dependencies, indirectAssetReferences))
        manager->AddAsset<AssetAttachment>(assetName, attachment, std::move(dependencies), std::vector<scr_string_t>(), std::move(indirectAssetReferences));
    else
        std::cerr << "Failed to load attachment \"" << assetName << "\"\n";

    return true;
}
