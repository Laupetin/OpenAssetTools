#include "DumperMaterialT6.h"

#include "Game/T6/Material/JsonMaterialWriterT6.h"
#include "Game/T6/Material/MaterialConstantZoneStateT6.h"
#include "Material/MaterialCommon.h"

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

    DumpMaterialAsJson(*assetFile, *asset->Asset(), context);
}
