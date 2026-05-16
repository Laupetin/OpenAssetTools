#include "FxImpactTableDumperIW3.h"

#include "CSV/CsvWriter.h"
#include "Utils/Logging/Log.h"

#include <Game/IW3/Fx/FxImpactTableConstantsIW3.h>
#include <format>

using namespace IW3;

namespace
{
    void DumpFxImpactEntry(const FxImpactEntry* subAsset, std::ostream& stream, const char* impactTypeName)
    {
        for (int i = 0; i < std::extent_v<decltype(fx_nonflesh_surface_type_names)>; i++)
        {
            const FxEffectDef* nonFleshDef = subAsset->nonflesh[i];
            stream << impactTypeName << ",";
            stream << fx_nonflesh_surface_type_names[i] << ",";
            if (nonFleshDef && nonFleshDef->name)
            {
                if (nonFleshDef->name[0] == ',')
                {
                    stream << nonFleshDef->name + 1;
                }
                else
                {
                    stream << nonFleshDef->name;
                }
            }
            stream << "\r\n";
        }

        for (int i = 0; i < std::extent_v<decltype(fx_flesh_surface_type_names)>; i++)
        {
            const FxEffectDef* fleshDef = subAsset->flesh[i];
            stream << impactTypeName << ",";
            stream << fx_flesh_surface_type_names[i] << ",";
            if (fleshDef && fleshDef->name)
            {
                if (fleshDef->name[0] == ',')
                {
                    stream << fleshDef->name + 1;
                }
                else
                {
                    stream << fleshDef->name;
                }
            }
            stream << "\r\n";
        }
    }

    void DumpFxImpactTable(const FxImpactTable* asset, std::ostream& stream)
    {
        const CsvWriter writer(stream);

        for (int i = 0; i < std::extent_v<decltype(fx_impact_type_names)>; i++)
        {
            const FxImpactEntry* tableEntry = (asset->table + i);
            if (tableEntry)
            {
                DumpFxImpactEntry(tableEntry, stream, fx_impact_type_names[i]);

                stream << "\r\n";
            }
        }
    }
} // namespace

namespace fx_impact_table
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<FxImpactTable>& asset)
    {
        const auto* fxImpactTable = asset.Asset();
        std::unique_ptr<std::ostream> assetFile = context.OpenAssetFile(std::format("fx/{}", context.m_zone.m_name + ".csv"));

        if (!assetFile)
            return;

        auto& stream = *assetFile;

        DumpFxImpactTable(fxImpactTable, stream);
    }
} // namespace fx_impact_table
