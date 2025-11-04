#include "CommonAssetT5.h"

#include "T5.h"

#include <cassert>

namespace T5
{
    CommonAssetTypeMapper::CommonAssetTypeMapper() = default;

    CommonAssetType CommonAssetTypeMapper::GameToCommonAssetType(const asset_type_t gameAssetType) const
    {
        static CommonAssetType lookupTable[static_cast<unsigned>(ASSET_TYPE_COUNT)]{
            CommonAssetType::XMODEL_PIECES,        // ASSET_TYPE_XMODELPIECES
            CommonAssetType::PHYS_PRESET,          // ASSET_TYPE_PHYSPRESET
            CommonAssetType::PHYS_CONSTRAINTS,     // ASSET_TYPE_PHYSCONSTRAINTS
            CommonAssetType::DESTRUCTIBLE_DEF,     // ASSET_TYPE_DESTRUCTIBLEDEF
            CommonAssetType::XANIM,                // ASSET_TYPE_XANIMPARTS
            CommonAssetType::XMODEL,               // ASSET_TYPE_XMODEL
            CommonAssetType::MATERIAL,             // ASSET_TYPE_MATERIAL
            CommonAssetType::TECHNIQUE_SET,        // ASSET_TYPE_TECHNIQUE_SET
            CommonAssetType::IMAGE,                // ASSET_TYPE_IMAGE
            CommonAssetType::SOUND,                // ASSET_TYPE_SOUND
            CommonAssetType::SOUND_PATCH,          // ASSET_TYPE_SOUND_PATCH
            CommonAssetType::CLIP_MAP,             // ASSET_TYPE_CLIPMAP
            CommonAssetType::CLIP_MAP,             // ASSET_TYPE_CLIPMAP_PVS
            CommonAssetType::COM_WORLD,            // ASSET_TYPE_COMWORLD
            CommonAssetType::GAME_WORLD_SP,        // ASSET_TYPE_GAMEWORLD_SP
            CommonAssetType::GAME_WORLD_MP,        // ASSET_TYPE_GAMEWORLD_MP
            CommonAssetType::MAP_ENTS,             // ASSET_TYPE_MAP_ENTS
            CommonAssetType::GFX_WORLD,            // ASSET_TYPE_GFXWORLD
            CommonAssetType::LIGHT_DEF,            // ASSET_TYPE_LIGHT_DEF
            CommonAssetType::UI_MAP,               // ASSET_TYPE_UI_MAP
            CommonAssetType::FONT,                 // ASSET_TYPE_FONT
            CommonAssetType::MENU_LIST,            // ASSET_TYPE_MENULIST
            CommonAssetType::MENU,                 // ASSET_TYPE_MENU
            CommonAssetType::LOCALIZE_ENTRY,       // ASSET_TYPE_LOCALIZE_ENTRY
            CommonAssetType::WEAPON,               // ASSET_TYPE_WEAPON
            CommonAssetType::WEAPON_DEF,           // ASSET_TYPE_WEAPONDEF
            CommonAssetType::WEAPON_VARIANT,       // ASSET_TYPE_WEAPON_VARIANT
            CommonAssetType::SOUND_DRIVER_GLOBALS, // ASSET_TYPE_SNDDRIVER_GLOBALS
            CommonAssetType::FX,                   // ASSET_TYPE_FX
            CommonAssetType::IMPACT_FX,            // ASSET_TYPE_IMPACT_FX
            CommonAssetType::AI_TYPE,              // ASSET_TYPE_AITYPE
            CommonAssetType::MP_TYPE,              // ASSET_TYPE_MPTYPE
            CommonAssetType::MP_BODY,              // ASSET_TYPE_MPBODY
            CommonAssetType::MP_HEAD,              // ASSET_TYPE_MPHEAD
            CommonAssetType::CHARACTER,            // ASSET_TYPE_CHARACTER
            CommonAssetType::XMODEL_ALIAS,         // ASSET_TYPE_XMODELALIAS
            CommonAssetType::RAW_FILE,             // ASSET_TYPE_RAWFILE
            CommonAssetType::STRING_TABLE,         // ASSET_TYPE_STRINGTABLE
            CommonAssetType::PACK_INDEX,           // ASSET_TYPE_PACK_INDEX
            CommonAssetType::XGLOBALS,             // ASSET_TYPE_XGLOBALS
            CommonAssetType::DDL,                  // ASSET_TYPE_DDL
            CommonAssetType::GLASSES,              // ASSET_TYPE_GLASSES
            CommonAssetType::EMBLEM_SET,           // ASSET_TYPE_EMBLEMSET
        };

        assert(gameAssetType < ASSET_TYPE_COUNT);
        return lookupTable[gameAssetType];
    }

    std::optional<asset_type_t> CommonAssetTypeMapper::CommonToGameAssetType(const CommonAssetType commonAssetType) const
    {
#define MAP_COMMON(common, game)                                                                                                                               \
    case common:                                                                                                                                               \
        return game;

        switch (commonAssetType)
        {
            MAP_COMMON(CommonAssetType::XMODEL_PIECES, ASSET_TYPE_XMODELPIECES)
            MAP_COMMON(CommonAssetType::PHYS_PRESET, ASSET_TYPE_PHYSPRESET)
            MAP_COMMON(CommonAssetType::PHYS_CONSTRAINTS, ASSET_TYPE_PHYSCONSTRAINTS)
            MAP_COMMON(CommonAssetType::DESTRUCTIBLE_DEF, ASSET_TYPE_DESTRUCTIBLEDEF)
            MAP_COMMON(CommonAssetType::XANIM, ASSET_TYPE_XANIMPARTS)
            MAP_COMMON(CommonAssetType::XMODEL, ASSET_TYPE_XMODEL)
            MAP_COMMON(CommonAssetType::MATERIAL, ASSET_TYPE_MATERIAL)
            MAP_COMMON(CommonAssetType::TECHNIQUE_SET, ASSET_TYPE_TECHNIQUE_SET)
            MAP_COMMON(CommonAssetType::IMAGE, ASSET_TYPE_IMAGE)
            MAP_COMMON(CommonAssetType::SOUND, ASSET_TYPE_SOUND)
            MAP_COMMON(CommonAssetType::SOUND_PATCH, ASSET_TYPE_SOUND_PATCH)
            MAP_COMMON(CommonAssetType::CLIP_MAP, ASSET_TYPE_CLIPMAP_PVS)
            MAP_COMMON(CommonAssetType::COM_WORLD, ASSET_TYPE_COMWORLD)
            MAP_COMMON(CommonAssetType::GAME_WORLD_SP, ASSET_TYPE_GAMEWORLD_SP)
            MAP_COMMON(CommonAssetType::GAME_WORLD_MP, ASSET_TYPE_GAMEWORLD_MP)
            MAP_COMMON(CommonAssetType::MAP_ENTS, ASSET_TYPE_MAP_ENTS)
            MAP_COMMON(CommonAssetType::GFX_WORLD, ASSET_TYPE_GFXWORLD)
            MAP_COMMON(CommonAssetType::LIGHT_DEF, ASSET_TYPE_LIGHT_DEF)
            MAP_COMMON(CommonAssetType::UI_MAP, ASSET_TYPE_UI_MAP)
            MAP_COMMON(CommonAssetType::FONT, ASSET_TYPE_FONT)
            MAP_COMMON(CommonAssetType::MENU_LIST, ASSET_TYPE_MENULIST)
            MAP_COMMON(CommonAssetType::MENU, ASSET_TYPE_MENU)
            MAP_COMMON(CommonAssetType::LOCALIZE_ENTRY, ASSET_TYPE_LOCALIZE_ENTRY)
            MAP_COMMON(CommonAssetType::WEAPON, ASSET_TYPE_WEAPON)
            MAP_COMMON(CommonAssetType::WEAPON_DEF, ASSET_TYPE_WEAPONDEF)
            MAP_COMMON(CommonAssetType::WEAPON_VARIANT, ASSET_TYPE_WEAPON_VARIANT)
            MAP_COMMON(CommonAssetType::SOUND_DRIVER_GLOBALS, ASSET_TYPE_SNDDRIVER_GLOBALS)
            MAP_COMMON(CommonAssetType::FX, ASSET_TYPE_FX)
            MAP_COMMON(CommonAssetType::IMPACT_FX, ASSET_TYPE_IMPACT_FX)
            MAP_COMMON(CommonAssetType::AI_TYPE, ASSET_TYPE_AITYPE)
            MAP_COMMON(CommonAssetType::MP_TYPE, ASSET_TYPE_MPTYPE)
            MAP_COMMON(CommonAssetType::MP_BODY, ASSET_TYPE_MPBODY)
            MAP_COMMON(CommonAssetType::MP_HEAD, ASSET_TYPE_MPHEAD)
            MAP_COMMON(CommonAssetType::CHARACTER, ASSET_TYPE_CHARACTER)
            MAP_COMMON(CommonAssetType::XMODEL_ALIAS, ASSET_TYPE_XMODELALIAS)
            MAP_COMMON(CommonAssetType::RAW_FILE, ASSET_TYPE_RAWFILE)
            MAP_COMMON(CommonAssetType::STRING_TABLE, ASSET_TYPE_STRINGTABLE)
            MAP_COMMON(CommonAssetType::PACK_INDEX, ASSET_TYPE_PACK_INDEX)
            MAP_COMMON(CommonAssetType::XGLOBALS, ASSET_TYPE_XGLOBALS)
            MAP_COMMON(CommonAssetType::DDL, ASSET_TYPE_DDL)
            MAP_COMMON(CommonAssetType::GLASSES, ASSET_TYPE_GLASSES)
            MAP_COMMON(CommonAssetType::EMBLEM_SET, ASSET_TYPE_EMBLEMSET)

        default:
            return std::nullopt;
        }

#undef MAP_COMMON
    }
} // namespace T5
