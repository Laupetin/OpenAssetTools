#include "DumperMaterialT5.h"

#include "Game/T5/Material/JsonMaterialWriterT5.h"
#include "Game/T5/Material/MaterialConstantZoneStateT5.h"
#include "Material/MaterialCommon.h"

#include <assert.h>

using namespace T5;

void AssetDumperMaterial::DumpPool(AssetDumpingContext& context, AssetPool<Material>* pool)
{
    auto* materialConstantState = context.GetZoneAssetDumperState<MaterialConstantZoneState>();
    materialConstantState->ExtractNamesFromZone();

    AbstractAssetDumper::DumpPool(context, pool);
}

bool AssetDumperMaterial::ShouldDump(XAssetInfo<Material>* asset)
{
    return true;
}

void AssetDumperMaterial::DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset)
{
    const auto assetFile = context.OpenAssetFile(material::GetFileNameForAssetName(asset->m_name));

    if (!assetFile)
        return;

    const auto* material = asset->Asset();
    assert(material->info.gameFlags < 0x400);
    assert(material->maxStreamedMips == 0);
    DumpMaterialAsJson(*assetFile, *material, context);
}
