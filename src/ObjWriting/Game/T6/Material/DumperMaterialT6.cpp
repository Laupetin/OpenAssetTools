#include "DumperMaterialT6.h"

#include "Game/T6/Material/JsonMaterialWriterT6.h"
#include "Game/T6/Material/MaterialConstantZoneStateT6.h"
#include "Material/MaterialCommon.h"

#include <cassert>

using namespace T6;

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
    assert(material->info.gameFlags < 0x8000);
    DumpMaterialAsJson(*assetFile, *material, context);
}
