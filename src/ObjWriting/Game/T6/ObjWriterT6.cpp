#include "ObjWriterT6.h"

#include "FontIcon/FontIconDumperT6.h"
#include "Game/T6/Material/MaterialJsonDumperT6.h"
#include "Game/T6/XModel/XModelDumperT6.h"
#include "Image/ImageDumperT6.h"
#include "Leaderboard/LeaderboardJsonDumperT6.h"
#include "Localize/LocalizeDumperT6.h"
#include "Maps/MapEntsDumperT6.h"
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

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<phys_preset::InfoStringDumperT6>());
    RegisterAssetDumper(std::make_unique<phys_constraints::InfoStringDumperT6>());
    // REGISTER_DUMPER(AssetDumperDestructibleDef, m_destructible_def)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    RegisterAssetDumper(std::make_unique<xmodel::DumperT6>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperT6>());
    RegisterAssetDumper(std::make_unique<techset::DumperT6>(
#ifdef TECHSET_DEBUG
        true
#else
        false
#endif
        ));
    RegisterAssetDumper(std::make_unique<image::DumperT6>());
    RegisterAssetDumper(std::make_unique<sound::SndBankDumperT6>());
    // REGISTER_DUMPER(AssetDumperSndPatch, m_sound_patch)
    // REGISTER_DUMPER(AssetDumperClipMap, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGameWorldSp, m_game_world_sp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp, m_game_world_mp)
    RegisterAssetDumper(std::make_unique<map_ents::DumperT6>());
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    // REGISTER_DUMPER(AssetDumperGfxLightDef, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont, m_font)
    RegisterAssetDumper(font_icon::CreateDumperT6());
    // REGISTER_DUMPER(AssetDumperMenuList, m_menu_list)
    // REGISTER_DUMPER(AssetDumperMenuDef, m_menu_def)
    RegisterAssetDumper(std::make_unique<localize::DumperT6>());
    RegisterAssetDumper(std::make_unique<weapon::DumperT6>());
    RegisterAssetDumper(std::make_unique<attachment::DumperT6>());
    RegisterAssetDumper(std::make_unique<attachment_unique::DumperT6>());
    RegisterAssetDumper(std::make_unique<camo::JsonDumperT6>());
    RegisterAssetDumper(std::make_unique<sound::SndDriverGlobalsDumperT6>());
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    RegisterAssetDumper(std::make_unique<raw_file::DumperT6>());
    RegisterAssetDumper(std::make_unique<string_table::DumperT6>());
    RegisterAssetDumper(std::make_unique<leaderboard::JsonDumperT6>());
    // REGISTER_DUMPER(AssetDumperXGlobals, m_xglobals)
    // REGISTER_DUMPER(AssetDumperDDLRoot, m_ddl)
    // REGISTER_DUMPER(AssetDumperGlasses, m_glasses)
    // REGISTER_DUMPER(AssetDumperEmblemSet, m_emblem_set)
    RegisterAssetDumper(std::make_unique<script::DumperT6>());
    // REGISTER_DUMPER(AssetDumperKeyValuePairs, m_key_value_pairs)
    RegisterAssetDumper(std::make_unique<vehicle::DumperT6>());
    // REGISTER_DUMPER(AssetDumperMemoryBlock, m_memory_block)
    // REGISTER_DUMPER(AssetDumperAddonMapEnts, m_addon_map_ents)
    RegisterAssetDumper(std::make_unique<tracer::DumperT6>());
    // REGISTER_DUMPER(AssetDumperSkinnedVertsDef, m_skinned_verts)
    RegisterAssetDumper(std::make_unique<qdb::DumperT6>());
    RegisterAssetDumper(std::make_unique<slug::DumperT6>());
    // REGISTER_DUMPER(AssetDumperFootstepTableDef, m_footstep_table)
    // REGISTER_DUMPER(AssetDumperFootstepFXTableDef, m_footstep_fx_table)
    RegisterAssetDumper(std::make_unique<z_barrier::DumperT6>());
}
