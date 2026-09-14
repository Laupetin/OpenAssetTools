#include "FxImpactTableDumperIW3.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/MaterialConstantsIW3.h"

#include <array>
#include <format>
#include <string_view>

using namespace IW3;

namespace
{
    constexpr std::array IMPACT_TYPE_NAMES{
        "bullet_small_normal",
        "bullet_small_exit",
        "bullet_large_normal",
        "bullet_large_exit",
        "shotgun_normal",
        "shotgun_exit",
        "bullet_ap_normal",
        "bullet_ap_exit",
        "grenade_bounce",
        "grenade_explode",
        "rocket_explode",
        "projectile_dud",
    };

    constexpr std::array FLESH_TYPE_NAMES{
        "flesh_body_nonfatal",
        "flesh_body_fatal",
        "flesh_head_nonfatal",
        "flesh_head_fatal",
    };

    const char* EffectName(const FxEffectDef* effect)
    {
        if (!effect || !effect->name)
            return "";

        return effect->name[0] == ',' ? &effect->name[1] : effect->name;
    }

    bool IsCommonZone(const std::string_view zoneName)
    {
        return zoneName == "common" || zoneName == "common_mp";
    }
} // namespace

namespace fx
{
    bool ImpactTableDumperIW3::ShouldDump(const XAssetInfo<AssetImpactFx::Type>& asset)
    {
        // The linker creates this special asset without reading a CSV file.
        return asset.m_name != "default";
    }

    void ImpactTableDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetImpactFx::Type>& asset)
    {
        const auto* impactTable = asset.Asset();
        if (!impactTable->table)
        {
            con::error("FxImpactTable \"{}\" has no impact entries", asset.m_name);
            return;
        }

        const auto isRootTable = asset.m_name == "void" || (asset.m_name.empty() && IsCommonZone(context.m_zone.m_name));
        const auto mapName = asset.m_name.empty() ? context.m_zone.m_name : asset.m_name;
        const auto fileName = isRootTable ? "fx/iw_impacts.csv" : std::format("fx/maps/{}/iw_impacts.csv", mapName);
        const auto assetFile = context.OpenAssetFile(fileName);
        if (!assetFile)
            return;

        *assetFile << "# Decompiled from the resolved impact table.\n";
        CsvOutputStream csv(*assetFile);
        for (auto impactIndex = 0u; impactIndex < IMPACT_TYPE_NAMES.size(); impactIndex++)
        {
            const auto& entry = impactTable->table[impactIndex];
            for (auto surfaceIndex = 0u; surfaceIndex < std::size(surfaceTypeNames); surfaceIndex++)
            {
                csv.WriteColumn(IMPACT_TYPE_NAMES[impactIndex]);
                csv.WriteColumn(surfaceTypeNames[surfaceIndex]);
                csv.WriteColumn(EffectName(entry.nonflesh[surfaceIndex]));
                csv.NextRow();
            }

            for (auto fleshIndex = 0u; fleshIndex < FLESH_TYPE_NAMES.size(); fleshIndex++)
            {
                csv.WriteColumn(IMPACT_TYPE_NAMES[impactIndex]);
                csv.WriteColumn(FLESH_TYPE_NAMES[fleshIndex]);
                csv.WriteColumn(EffectName(entry.flesh[fleshIndex]));
                csv.NextRow();
            }
        }
    }
} // namespace fx
