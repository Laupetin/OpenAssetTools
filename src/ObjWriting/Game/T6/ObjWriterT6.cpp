#include "ObjWriterT6.h"

#include "FontIcon/FontIconDumperT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/Material/MaterialJsonDumperT6.h"
#include "Game/T6/XModel/XModelDumperT6.h"
#include "Image/ImageDumperT6.h"
#include "Leaderboard/LeaderboardJsonDumperT6.h"
#include "Localize/LocalizeDumperT6.h"
#include "Maps/MapEntsDumperT6.h"
#include "ObjWriting.h"
#include "PhysConstraints/PhysConstraintsInfoStringDumperT6.h"
#include "PhysPreset/PhysPresetInfoStringDumperT6.h"
#include "Qdb/QdbDumperT6.h"
#include "RawFile/RawFileDumperT6.h"
#include "Script/ScriptDumperT6.h"
#include "Slug/SlugDumperT6.h"
#include "Sound/SndBankDumperT6.h"
#include "Sound/SndDriverGlobalsDumperT6.h"
#include "StringTable/StringTableDumperT6.h"
#include "Techset/TechsetDumperT6.h"
#include "Tracer/TracerDumperT6.h"
#include "Vehicle/VehicleDumperT6.h"
#include "Weapon/AttachmentDumperT6.h"
#include "Weapon/AttachmentUniqueDumperT6.h"
#include "Weapon/CamoJsonDumperT6.h"
#include "Weapon/WeaponDumperT6.h"
#include "ZBarrier/ZBarrierDumperT6.h"

using namespace T6;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define REGISTER_DUMPER(dumperType, poolName)                                                                                                                  \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(dumperType::AssetType_t::EnumEntry))                                                         \
    {                                                                                                                                                          \
        dumpers.emplace_back(std::make_unique<dumperType>(*assetPools->poolName));                                                                             \
    }
#define REGISTER_DUMPER_WITH_FACTORY(createDumper, poolName, asset)                                                                                            \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(asset::EnumEntry))                                                                           \
    {                                                                                                                                                          \
        auto dumper = createDumper(*assetPools->poolName);                                                                                                     \
        if (dumper)                                                                                                                                            \
            dumpers.emplace_back(std::move(dumper));                                                                                                           \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolT6*>(context.m_zone.m_pools.get());
    std::vector<std::unique_ptr<IAssetDumper>> dumpers;

    REGISTER_DUMPER(phys_preset::InfoStringDumperT6, m_phys_preset)
    REGISTER_DUMPER(phys_constraints::InfoStringDumperT6, m_phys_constraints)
    // REGISTER_DUMPER(AssetDumperDestructibleDef, m_destructible_def)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    REGISTER_DUMPER(xmodel::DumperT6, m_xmodel)
    REGISTER_DUMPER(material::JsonDumperT6, m_material)
    REGISTER_DUMPER(techset::DumperT6, m_technique_set)
    REGISTER_DUMPER(image::DumperT6, m_image)
    REGISTER_DUMPER(sound::SndBankDumperT6, m_sound_bank)
    // REGISTER_DUMPER(AssetDumperSndPatch, m_sound_patch)
    // REGISTER_DUMPER(AssetDumperClipMap, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGameWorldSp, m_game_world_sp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp, m_game_world_mp)
    REGISTER_DUMPER(map_ents::DumperT6, m_map_ents)
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    // REGISTER_DUMPER(AssetDumperGfxLightDef, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont, m_font)
    REGISTER_DUMPER_WITH_FACTORY(font_icon::CreateDumperT6, m_font_icon, AssetFontIcon)
    // REGISTER_DUMPER(AssetDumperMenuList, m_menu_list)
    // REGISTER_DUMPER(AssetDumperMenuDef, m_menu_def)
    REGISTER_DUMPER(localize::DumperT6, m_localize)
    REGISTER_DUMPER(weapon::DumperT6, m_weapon)
    REGISTER_DUMPER(attachment::DumperT6, m_attachment)
    REGISTER_DUMPER(attachment_unique::DumperT6, m_attachment_unique)
    REGISTER_DUMPER(camo::JsonDumperT6, m_camo)
    REGISTER_DUMPER(sound::SndDriverGlobalsDumperT6, m_snd_driver_globals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    REGISTER_DUMPER(raw_file::DumperT6, m_raw_file)
    REGISTER_DUMPER(string_table::DumperT6, m_string_table)
    REGISTER_DUMPER(leaderboard::JsonDumperT6, m_leaderboard)
    // REGISTER_DUMPER(AssetDumperXGlobals, m_xglobals)
    // REGISTER_DUMPER(AssetDumperDDLRoot, m_ddl)
    // REGISTER_DUMPER(AssetDumperGlasses, m_glasses)
    // REGISTER_DUMPER(AssetDumperEmblemSet, m_emblem_set)
    REGISTER_DUMPER(script::DumperT6, m_script)
    // REGISTER_DUMPER(AssetDumperKeyValuePairs, m_key_value_pairs)
    REGISTER_DUMPER(vehicle::DumperT6, m_vehicle)
    // REGISTER_DUMPER(AssetDumperMemoryBlock, m_memory_block)
    // REGISTER_DUMPER(AssetDumperAddonMapEnts, m_addon_map_ents)
    REGISTER_DUMPER(tracer::DumperT6, m_tracer)
    // REGISTER_DUMPER(AssetDumperSkinnedVertsDef, m_skinned_verts)
    REGISTER_DUMPER(qdb::DumperT6, m_qdb)
    REGISTER_DUMPER(slug::DumperT6, m_slug)
    // REGISTER_DUMPER(AssetDumperFootstepTableDef, m_footstep_table)
    // REGISTER_DUMPER(AssetDumperFootstepFXTableDef, m_footstep_fx_table)
    REGISTER_DUMPER(z_barrier::DumperT6, m_zbarrier)

    if (context.ShouldTrackProgress())
    {
        size_t totalProgress = 0uz;
        for (const auto& dumper : dumpers)
            totalProgress += dumper->GetProgressTotalCount();

        context.SetTotalProgress(totalProgress);
    }

    for (const auto& dumper : dumpers)
        dumper->Dump(context);

    return true;

#undef REGISTER_DUMPER
}
