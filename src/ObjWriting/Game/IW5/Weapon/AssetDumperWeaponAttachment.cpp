#include "AssetDumperWeaponAttachment.h"

#include "Game/IW5/Weapon/JsonWeaponAttachmentWriter.h"

#include <format>

using namespace IW5;

bool AssetDumperWeaponAttachment::ShouldDump(XAssetInfo<WeaponAttachment>* asset)
{
    return true;
}

void AssetDumperWeaponAttachment::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset)
{
    const auto assetFile = context.OpenAssetFile(std::format("attachment/{}.json", asset->m_name));

    if (!assetFile)
        return;

    DumpWeaponAttachmentAsJson(*assetFile, asset->Asset(), context);
}
