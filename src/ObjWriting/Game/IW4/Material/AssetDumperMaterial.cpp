#include "AssetDumperMaterial.h"

#include "DecompilingMaterialDumperIW4.h"

using namespace IW4;

bool AssetDumperMaterial::ShouldDump(XAssetInfo<Material>* asset)
{
    return true;
}

void AssetDumperMaterial::DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset)
{
    const auto* material = asset->Asset();

    if (context.m_gdt)
    {
        DecompileMaterialToGdt(*context.m_gdt, *material, context);
    }
}
