#include "AssetDumperStructuredDataDefSet.h"

#include <cassert>
#include <sstream>

#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"

using namespace IW4;

bool AssetDumperStructuredDataDefSet::ShouldDump(XAssetInfo<StructuredDataDefSet>* asset)
{
    return true;
}

void AssetDumperStructuredDataDefSet::DumpEnum(StructuredDataDefDumper& dumper, const int enumIndex, const StructuredDataEnum* _enum)
{
    if (!_enum->entries || _enum->entryCount <= 0)
        return;

    std::ostringstream ss;
    ss << "ENUM_" << enumIndex;

    dumper.BeginEnum(ss.str(), static_cast<size_t>(_enum->entryCount));

    for(auto i = 0; i < _enum->entryCount; i++)
    {
        const auto& entry = _enum->entries[i];

        assert(entry.string);
        if(!entry.string)
            continue;

        dumper.WriteEnumEntry(entry.string, entry.index);
    }

    dumper.EndEnum();
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

        for (auto enumIndex = 0; enumIndex < def.enumCount; enumIndex++)
            DumpEnum(dumper, enumIndex, &def.enums[enumIndex]);

        // TODO

        dumper.EndVersion();
    }
}
