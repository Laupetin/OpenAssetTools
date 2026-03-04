#include "ObjWriterT5.h"

#include "Game/T5/Material/MaterialJsonDumperT5.h"
#include "Game/T5/Techset/TechsetDumperT5.h"
#include "Game/T5/XModel/XModelDumperT5.h"
#include "Image/ImageDumperT5.h"
#include "Localize/LocalizeDumperT5.h"
#include "RawFile/RawFileDumperT5.h"
#include "StringTable/StringTableDumperT5.h"

using namespace T5;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    // REGISTER_DUMPER(AssetDumperPhysPreset, m_phys_preset)
    // REGISTER_DUMPER(AssetDumperPhysConstraints, m_phys_constraints)
    // REGISTER_DUMPER(AssetDumperDestructibleDef, m_destructible_def)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    RegisterAssetDumper(std::make_unique<xmodel::DumperT5>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperT5>());
    RegisterAssetDumper(std::make_unique<techset::DumperT5>(
#ifdef TECHSET_DEBUG
        true
#else
        false
#endif
        ));
    RegisterAssetDumper(std::make_unique<image::DumperT5>());
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
    RegisterAssetDumper(std::make_unique<localize::DumperT5>());
    // REGISTER_DUMPER(AssetDumperWeapon, m_weapon)
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals, m_snd_driver_globals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    RegisterAssetDumper(std::make_unique<raw_file::DumperT5>());
    RegisterAssetDumper(std::make_unique<string_table::DumperT5>());
    // REGISTER_DUMPER(AssetDumperPackIndex, m_pack_index)
    // REGISTER_DUMPER(AssetDumperXGlobals, m_xglobals)
    // REGISTER_DUMPER(AssetDumperDDLRoot, m_ddl)
    // REGISTER_DUMPER(AssetDumperGlasses, m_glasses)
    // REGISTER_DUMPER(AssetDumperEmblemSet, m_emblem_set)
}
