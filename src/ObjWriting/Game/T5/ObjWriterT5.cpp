#include "ObjWriterT5.h"

#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T5/Material/MaterialJsonDumperT5.h"
#include "Game/T5/Techset/TechsetDumperT5.h"
#include "Game/T5/XModel/XModelDumperT5.h"
#include "Image/ImageDumperT5.h"
#include "Localize/LocalizeDumperT5.h"
#include "ObjWriting.h"
#include "RawFile/RawFileDumperT5.h"
#include "StringTable/StringTableDumperT5.h"

using namespace T5;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define REGISTER_DUMPER(dumperType, poolName)                                                                                                                  \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(dumperType::AssetType_t::EnumEntry))                                                         \
    {                                                                                                                                                          \
        dumpers.emplace_back(std::make_unique<dumperType>(*assetPools->poolName));                                                                             \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolT5*>(context.m_zone.m_pools.get());
    std::vector<std::unique_ptr<IAssetDumper>> dumpers;

    // REGISTER_DUMPER(AssetDumperPhysPreset, m_phys_preset)
    // REGISTER_DUMPER(AssetDumperPhysConstraints, m_phys_constraints)
    // REGISTER_DUMPER(AssetDumperDestructibleDef, m_destructible_def)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    REGISTER_DUMPER(xmodel::DumperT5, m_xmodel)
    REGISTER_DUMPER(material::JsonDumperT5, m_material)
    REGISTER_DUMPER(techset::DumperT5, m_technique_set)
    REGISTER_DUMPER(image::DumperT5, m_image)
    // REGISTER_DUMPER(AssetDumperSndBank, m_sound_bank)
    // REGISTER_DUMPER(AssetDumperSndPatch, m_sound_patch)
    // REGISTER_DUMPER(AssetDumperClipMap, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGameWorldSp, m_game_world_sp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp, m_game_world_mp)
    // REGISTER_DUMPER(AssetDumperMapEnts, m_map_ents)
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    // REGISTER_DUMPER(AssetDumperGfxLightDef, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont, m_font)
    // REGISTER_DUMPER(AssetDumperMenuList, m_menu_list)
    // REGISTER_DUMPER(AssetDumperMenuDef, m_menu_def)
    REGISTER_DUMPER(localize::DumperT5, m_localize)
    // REGISTER_DUMPER(AssetDumperWeapon, m_weapon)
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals, m_snd_driver_globals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    REGISTER_DUMPER(raw_file::DumperT5, m_raw_file)
    REGISTER_DUMPER(string_table::DumperT5, m_string_table)
    // REGISTER_DUMPER(AssetDumperPackIndex, m_pack_index)
    // REGISTER_DUMPER(AssetDumperXGlobals, m_xglobals)
    // REGISTER_DUMPER(AssetDumperDDLRoot, m_ddl)
    // REGISTER_DUMPER(AssetDumperGlasses, m_glasses)
    // REGISTER_DUMPER(AssetDumperEmblemSet, m_emblem_set)

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
