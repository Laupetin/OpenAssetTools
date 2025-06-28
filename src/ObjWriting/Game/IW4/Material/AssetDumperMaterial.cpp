#include "AssetDumperMaterial.h"

#include "DecompilingMaterialDumperIW4.h"
#include "Game/IW4/Material/JsonMaterialWriterIW4.h"
#include "Game/IW4/Material/MaterialConstantZoneState.h"
#include "Material/MaterialCommon.h"

using namespace IW4;

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
#ifdef EXPERIMENTAL_MATERIAL_COMPILATION
    if (context.m_gdt)
    {
        DecompileMaterialToGdt(*context.m_gdt, *asset->Asset(), context);
    }
#else
    const auto assetFile = context.OpenAssetFile(material::GetFileNameForAssetName(asset->m_name));

    if (!assetFile)
        return;

    DumpMaterialAsJson(*assetFile, *asset->Asset(), context);
#endif
}
