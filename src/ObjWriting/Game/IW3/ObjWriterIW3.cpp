#include "ObjWriterIW3.h"

#include "Game/IW3/GameAssetPoolIW3.h"
#include "Game/IW3/Material/MaterialJsonDumperIW3.h"
#include "Game/IW3/XModel/XModelDumperIW3.h"
#include "Image/ImageDumperIW3.h"
#include "Localize/LocalizeDumperIW3.h"
#include "Maps/MapEntsDumperIW3.h"
#include "ObjWriting.h"
#include "RawFile/RawFileDumperIW3.h"
#include "Sound/LoadedSoundDumperIW3.h"
#include "StringTable/StringTableDumperIW3.h"

using namespace IW3;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName, assetType)                                                                                                       \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(assetType))                                                                                  \
    {                                                                                                                                                          \
        dumperType dumper;                                                                                                                                     \
        dumper.DumpPool(context, assetPools->poolName.get());                                                                                                  \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW3*>(context.m_zone.m_pools.get());

    // DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset, ASSET_TYPE_PHYSPRESET)
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts, ASSET_TYPE_XANIMPARTS)
    DUMP_ASSET_POOL(xmodel::Dumper, m_xmodel, ASSET_TYPE_XMODEL)
    DUMP_ASSET_POOL(material::JsonDumper, m_material, ASSET_TYPE_MATERIAL)
    // DUMP_ASSET_POOL(AssetDumperMaterialTechniqueSet, m_technique_set, ASSET_TYPE_TECHNIQUE_SET)
    DUMP_ASSET_POOL(image::Dumper, m_image, ASSET_TYPE_IMAGE)
    // DUMP_ASSET_POOL(AssetDumpersnd_alias_list_t, m_sound, ASSET_TYPE_SOUND)
    // DUMP_ASSET_POOL(AssetDumperSndCurve, m_sound_curve, ASSET_TYPE_SOUND_CURVE)
    DUMP_ASSET_POOL(sound::LoadedSoundDumper, m_loaded_sound, ASSET_TYPE_LOADED_SOUND)
    // DUMP_ASSET_POOL(AssetDumperClipMap, m_clip_map, ASSET_TYPE_CLIPMAP_PVS)
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world, ASSET_TYPE_COMWORLD)
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp, ASSET_TYPE_GAMEWORLD_SP)
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp, ASSET_TYPE_GAMEWORLD_MP)
    DUMP_ASSET_POOL(map_ents::Dumper, m_map_ents, ASSET_TYPE_MAP_ENTS)
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world, ASSET_TYPE_GFXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def, ASSET_TYPE_LIGHT_DEF)
    // DUMP_ASSET_POOL(AssetDumperFont_s, m_font, ASSET_TYPE_FONT)
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list, ASSET_TYPE_MENULIST)
    // DUMP_ASSET_POOL(AssetDumpermenuDef_t, m_menu_def, ASSET_TYPE_MENU)
    DUMP_ASSET_POOL(localize::Dumper, m_localize, ASSET_TYPE_LOCALIZE_ENTRY)
    // DUMP_ASSET_POOL(AssetDumperWeapon, m_weapon, ASSET_TYPE_WEAPON)
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals, ASSET_TYPE_SNDDRIVER_GLOBALS)
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx, ASSET_TYPE_FX)
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table, ASSET_TYPE_IMPACT_FX)
    DUMP_ASSET_POOL(raw_file::Dumper, m_raw_file, ASSET_TYPE_RAWFILE)
    DUMP_ASSET_POOL(string_table::Dumper, m_string_table, ASSET_TYPE_STRINGTABLE)

    return true;

#undef DUMP_ASSET_POOL
}
