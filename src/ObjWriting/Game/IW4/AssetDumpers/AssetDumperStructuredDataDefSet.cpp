#include "AssetDumperStructuredDataDefSet.h"

#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"

using namespace IW4;

bool AssetDumperStructuredDataDefSet::ShouldDump(XAssetInfo<StructuredDataDefSet>* asset)
{
    return true;
}

void AssetDumperStructuredDataDefSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset)
{
    const auto* set = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile || set->defs == nullptr)
        return;

    StructuredDataDefDumper dumper(*assetFile);

    for(auto defIndex = 0u; defIndex < set->defCount; defIndex++)
    {
        const auto& def = set->defs[defIndex];

        dumper.BeginVersion(def.version);

        // TODO

        dumper.EndVersion();
    }
}
