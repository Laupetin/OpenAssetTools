#include "FxImpactTableDumperIW3.h"

#include "CSV/CsvWriter.h"
#include "Utils/Logging/Log.h"

#include <Game/IW3/FxImpactTable/FxImpactTableFields.h>
#include <format>

using namespace IW3;

namespace
{
    void DumpFxImpactEntry(const FxImpactEntry* subAsset, std::ostream& stream, const char* impactTypeName)
    {
        for (int i = 0; i < FX_IMPACT_ENTRY_NONFLESH_COUNT; i++)
        {
            const FxEffectDef* nonFleshDef = subAsset->nonflesh[i];
            stream << impactTypeName << ",";
            stream << fx_nonflesh_surface_type_names[i] << ",";
            if (nonFleshDef && nonFleshDef->name)
            {
                stream << nonFleshDef->name;
            }
            stream << "\r\n";
        }

        for (int i = 0; i < FX_IMPACT_ENTRY_FLESH_COUNT; i++)
        {
            const FxEffectDef* fleshDef = subAsset->flesh[i];
            stream << impactTypeName << ",";
            stream << fx_flesh_surface_type_names[i] << ",";
            if (fleshDef && fleshDef->name)
            {
                stream << fleshDef->name;
            }
            stream << "\r\n";
        }
    }

    void DumpFxImpactTable(const FxImpactTable* asset, std::ostream& stream)
    {
        const CsvWriter writer(stream);

        for (int i = 0; i < FX_IMPACT_TABLE_ENTRY_COUNT; i++)
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
        std::unique_ptr<std::ostream> assetFile;
        if (asset.m_name == "")
        {
            assetFile = context.OpenAssetFile(std::format("fx/iw_impacts.csv"));
        }
        else
        {
            assetFile = context.OpenAssetFile(std::format("fx/{}", asset.m_name + ".csv"));
        }

        if (!assetFile)
            return;

        auto& stream = *assetFile;

        DumpFxImpactTable(fxImpactTable, stream);
    }
} // namespace fx_impact_table
