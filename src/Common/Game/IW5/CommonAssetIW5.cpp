#include "CommonAssetIW5.h"

#include "IW5.h"

#include <cassert>

namespace IW5
{
    CommonAssetTypeMapper::CommonAssetTypeMapper() = default;

    CommonAssetType CommonAssetTypeMapper::GameToCommonAssetType(const asset_type_t gameAssetType) const
    {
        static CommonAssetType lookupTable[static_cast<unsigned>(ASSET_TYPE_COUNT)]{
            CommonAssetType::PHYS_PRESET,          // ASSET_TYPE_PHYSPRESET
            CommonAssetType::PHYS_COLL_MAP,        // ASSET_TYPE_PHYSCOLLMAP
            CommonAssetType::XANIM,                // ASSET_TYPE_XANIMPARTS
            CommonAssetType::XMODEL_SURFS,         // ASSET_TYPE_XMODEL_SURFS
            CommonAssetType::XMODEL,               // ASSET_TYPE_XMODEL
            CommonAssetType::MATERIAL,             // ASSET_TYPE_MATERIAL
            CommonAssetType::PIXEL_SHADER,         // ASSET_TYPE_PIXELSHADER
            CommonAssetType::VERTEX_SHADER,        // ASSET_TYPE_VERTEXSHADER
            CommonAssetType::VERTEX_DECL,          // ASSET_TYPE_VERTEXDECL
            CommonAssetType::TECHNIQUE_SET,        // ASSET_TYPE_TECHNIQUE_SET
            CommonAssetType::IMAGE,                // ASSET_TYPE_IMAGE
            CommonAssetType::SOUND,                // ASSET_TYPE_SOUND
            CommonAssetType::SOUND_CURVE,          // ASSET_TYPE_SOUND_CURVE
            CommonAssetType::LOADED_SOUND,         // ASSET_TYPE_LOADED_SOUND
            CommonAssetType::CLIP_MAP,             // ASSET_TYPE_CLIPMAP
            CommonAssetType::COM_WORLD,            // ASSET_TYPE_COMWORLD
            CommonAssetType::GLASS_WORLD,          // ASSET_TYPE_GLASSWORLD
            CommonAssetType::PATH_DATA,            // ASSET_TYPE_PATHDATA
            CommonAssetType::VEHICLE_TRACK,        // ASSET_TYPE_VEHICLE_TRACK
            CommonAssetType::MAP_ENTS,             // ASSET_TYPE_MAP_ENTS
            CommonAssetType::FX_WORLD,             // ASSET_TYPE_FXWORLD
            CommonAssetType::GFX_WORLD,            // ASSET_TYPE_GFXWORLD
            CommonAssetType::LIGHT_DEF,            // ASSET_TYPE_LIGHT_DEF
            CommonAssetType::UI_MAP,               // ASSET_TYPE_UI_MAP
            CommonAssetType::FONT,                 // ASSET_TYPE_FONT
            CommonAssetType::MENU_LIST,            // ASSET_TYPE_MENULIST
            CommonAssetType::MENU,                 // ASSET_TYPE_MENU
            CommonAssetType::LOCALIZE_ENTRY,       // ASSET_TYPE_LOCALIZE_ENTRY
            CommonAssetType::ATTACHMENT,           // ASSET_TYPE_ATTACHMENT
            CommonAssetType::WEAPON,               // ASSET_TYPE_WEAPON
            CommonAssetType::SOUND_DRIVER_GLOBALS, // ASSET_TYPE_SNDDRIVER_GLOBALS
            CommonAssetType::FX,                   // ASSET_TYPE_FX
            CommonAssetType::IMPACT_FX,            // ASSET_TYPE_IMPACT_FX
            CommonAssetType::SURFACE_FX,           // ASSET_TYPE_SURFACE_FX
            CommonAssetType::AI_TYPE,              // ASSET_TYPE_AITYPE
            CommonAssetType::MP_TYPE,              // ASSET_TYPE_MPTYPE
            CommonAssetType::CHARACTER,            // ASSET_TYPE_CHARACTER
            CommonAssetType::XMODEL_ALIAS,         // ASSET_TYPE_XMODELALIAS
            CommonAssetType::RAW_FILE,             // ASSET_TYPE_RAWFILE
            CommonAssetType::SCRIPT,               // ASSET_TYPE_SCRIPTFILE
            CommonAssetType::STRING_TABLE,         // ASSET_TYPE_STRINGTABLE
            CommonAssetType::LEADERBOARD,          // ASSET_TYPE_LEADERBOARD
            CommonAssetType::STRUCTURED_DATA_DEF,  // ASSET_TYPE_STRUCTURED_DATA_DEF
            CommonAssetType::TRACER,               // ASSET_TYPE_TRACER
            CommonAssetType::VEHICLE,              // ASSET_TYPE_VEHICLE
            CommonAssetType::ADDON_MAP_ENTS,       // ASSET_TYPE_ADDON_MAP_ENTS
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
            MAP_COMMON(CommonAssetType::PHYS_PRESET, ASSET_TYPE_PHYSPRESET)
            MAP_COMMON(CommonAssetType::PHYS_COLL_MAP, ASSET_TYPE_PHYSCOLLMAP)
            MAP_COMMON(CommonAssetType::XANIM, ASSET_TYPE_XANIMPARTS)
            MAP_COMMON(CommonAssetType::XMODEL_SURFS, ASSET_TYPE_XMODEL_SURFS)
            MAP_COMMON(CommonAssetType::XMODEL, ASSET_TYPE_XMODEL)
            MAP_COMMON(CommonAssetType::MATERIAL, ASSET_TYPE_MATERIAL)
            MAP_COMMON(CommonAssetType::PIXEL_SHADER, ASSET_TYPE_PIXELSHADER)
            MAP_COMMON(CommonAssetType::VERTEX_SHADER, ASSET_TYPE_VERTEXSHADER)
            MAP_COMMON(CommonAssetType::VERTEX_DECL, ASSET_TYPE_VERTEXDECL)
            MAP_COMMON(CommonAssetType::TECHNIQUE_SET, ASSET_TYPE_TECHNIQUE_SET)
            MAP_COMMON(CommonAssetType::IMAGE, ASSET_TYPE_IMAGE)
            MAP_COMMON(CommonAssetType::SOUND, ASSET_TYPE_SOUND)
            MAP_COMMON(CommonAssetType::SOUND_CURVE, ASSET_TYPE_SOUND_CURVE)
            MAP_COMMON(CommonAssetType::LOADED_SOUND, ASSET_TYPE_LOADED_SOUND)
            MAP_COMMON(CommonAssetType::CLIP_MAP, ASSET_TYPE_CLIPMAP)
            MAP_COMMON(CommonAssetType::COM_WORLD, ASSET_TYPE_COMWORLD)
            MAP_COMMON(CommonAssetType::GLASS_WORLD, ASSET_TYPE_GLASSWORLD)
            MAP_COMMON(CommonAssetType::PATH_DATA, ASSET_TYPE_PATHDATA)
            MAP_COMMON(CommonAssetType::VEHICLE_TRACK, ASSET_TYPE_VEHICLE_TRACK)
            MAP_COMMON(CommonAssetType::MAP_ENTS, ASSET_TYPE_MAP_ENTS)
            MAP_COMMON(CommonAssetType::FX_WORLD, ASSET_TYPE_FXWORLD)
            MAP_COMMON(CommonAssetType::GFX_WORLD, ASSET_TYPE_GFXWORLD)
            MAP_COMMON(CommonAssetType::LIGHT_DEF, ASSET_TYPE_LIGHT_DEF)
            MAP_COMMON(CommonAssetType::UI_MAP, ASSET_TYPE_UI_MAP)
            MAP_COMMON(CommonAssetType::FONT, ASSET_TYPE_FONT)
            MAP_COMMON(CommonAssetType::MENU_LIST, ASSET_TYPE_MENULIST)
            MAP_COMMON(CommonAssetType::MENU, ASSET_TYPE_MENU)
            MAP_COMMON(CommonAssetType::LOCALIZE_ENTRY, ASSET_TYPE_LOCALIZE_ENTRY)
            MAP_COMMON(CommonAssetType::ATTACHMENT, ASSET_TYPE_ATTACHMENT)
            MAP_COMMON(CommonAssetType::WEAPON, ASSET_TYPE_WEAPON)
            MAP_COMMON(CommonAssetType::SOUND_DRIVER_GLOBALS, ASSET_TYPE_SNDDRIVER_GLOBALS)
            MAP_COMMON(CommonAssetType::FX, ASSET_TYPE_FX)
            MAP_COMMON(CommonAssetType::IMPACT_FX, ASSET_TYPE_IMPACT_FX)
            MAP_COMMON(CommonAssetType::SURFACE_FX, ASSET_TYPE_SURFACE_FX)
            MAP_COMMON(CommonAssetType::AI_TYPE, ASSET_TYPE_AITYPE)
            MAP_COMMON(CommonAssetType::MP_TYPE, ASSET_TYPE_MPTYPE)
            MAP_COMMON(CommonAssetType::CHARACTER, ASSET_TYPE_CHARACTER)
            MAP_COMMON(CommonAssetType::XMODEL_ALIAS, ASSET_TYPE_XMODELALIAS)
            MAP_COMMON(CommonAssetType::RAW_FILE, ASSET_TYPE_RAWFILE)
            MAP_COMMON(CommonAssetType::SCRIPT, ASSET_TYPE_SCRIPTFILE)
            MAP_COMMON(CommonAssetType::STRING_TABLE, ASSET_TYPE_STRINGTABLE)
            MAP_COMMON(CommonAssetType::LEADERBOARD, ASSET_TYPE_LEADERBOARD)
            MAP_COMMON(CommonAssetType::STRUCTURED_DATA_DEF, ASSET_TYPE_STRUCTURED_DATA_DEF)
            MAP_COMMON(CommonAssetType::TRACER, ASSET_TYPE_TRACER)
            MAP_COMMON(CommonAssetType::VEHICLE, ASSET_TYPE_VEHICLE)
            MAP_COMMON(CommonAssetType::ADDON_MAP_ENTS, ASSET_TYPE_ADDON_MAP_ENTS)

        default:
            return std::nullopt;
        }

#undef MAP_COMMON
    }
} // namespace IW5
