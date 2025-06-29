#include "DumperMaterialIW5.h"

#include "Game/IW5/Material/JsonMaterialWriterIW5.h"
#include "Game/IW5/Material/MaterialConstantZoneStateIW5.h"
#include "Material/MaterialCommon.h"

using namespace IW5;

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
