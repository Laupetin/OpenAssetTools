#include "CommonAssetQOS.h"

#include "QOS.h"

#include <cassert>

namespace QOS
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
            CommonAssetType::SOUND_CURVE,          // ASSET_TYPE_SND_CURVE
            CommonAssetType::CLIP_MAP,             // ASSET_TYPE_CLIPMAP_SP
            CommonAssetType::CLIP_MAP,             // ASSET_TYPE_CLIPMAP_MP
            CommonAssetType::COM_WORLD,            // ASSET_TYPE_COMWORLD
            CommonAssetType::GAME_WORLD_SP,        // ASSET_TYPE_GAMEWORLD_SP
            CommonAssetType::GAME_WORLD_MP,        // ASSET_TYPE_GAMEWORLD_MP
            CommonAssetType::MAP_ENTS,             // ASSET_TYPE_MAP_ENTS
            CommonAssetType::GFX_WORLD,            // ASSET_TYPE_GFXWORLD
            CommonAssetType::LIGHT_DEF,            // ASSET_TYPE_LIGHT_DEF
            CommonAssetType::UI_MAP,               // ASSET_TYPE_UI_MAP
            CommonAssetType::FONT,                 // ASSET_TYPE_FONT
            CommonAssetType::MENU_LIST,            // ASSET_TYPE_MENUFILE
            CommonAssetType::MENU,                 // ASSET_TYPE_MENU
            CommonAssetType::LOCALIZE_ENTRY,       // ASSET_TYPE_LOCALIZE_ENTRY
            CommonAssetType::WEAPON,               // ASSET_TYPE_WEAPON
            CommonAssetType::SOUND_DRIVER_GLOBALS, // ASSET_TYPE_SNDDRIVER_GLOBALS
            CommonAssetType::FX,                   // ASSET_TYPE_FX
            CommonAssetType::IMPACT_FX,            // ASSET_TYPE_IMPACT_FX
            CommonAssetType::AI_TYPE,              // ASSET_TYPE_AITYPE
            CommonAssetType::MP_TYPE,              // ASSET_TYPE_MPTYPE
            CommonAssetType::CHARACTER,            // ASSET_TYPE_CHARACTER
            CommonAssetType::XMODEL_ALIAS,         // ASSET_TYPE_XMODELALIAS
            CommonAssetType::RAW_FILE,             // ASSET_TYPE_RAWFILE
            CommonAssetType::STRING_TABLE,         // ASSET_TYPE_STRINGTABLE
            CommonAssetType::XML_TREE,             // ASSET_TYPE_XMLTREE
            CommonAssetType::SCENE_ANIMATION,      // ASSET_TYPE_SCENE_ANIMATION
            CommonAssetType::CUTSCENE,             // ASSET_TYPE_CUTSCENE
            CommonAssetType::CUSTOM_CAMERA,        // ASSET_TYPE_CUSTOM_CAMERA
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
            MAP_COMMON(CommonAssetType::SOUND_CURVE, ASSET_TYPE_SND_CURVE)
            MAP_COMMON(CommonAssetType::CLIP_MAP, ASSET_TYPE_CLIPMAP_MP)
            MAP_COMMON(CommonAssetType::COM_WORLD, ASSET_TYPE_COMWORLD)
            MAP_COMMON(CommonAssetType::GAME_WORLD_SP, ASSET_TYPE_GAMEWORLD_SP)
            MAP_COMMON(CommonAssetType::GAME_WORLD_MP, ASSET_TYPE_GAMEWORLD_MP)
            MAP_COMMON(CommonAssetType::MAP_ENTS, ASSET_TYPE_MAP_ENTS)
            MAP_COMMON(CommonAssetType::GFX_WORLD, ASSET_TYPE_GFXWORLD)
            MAP_COMMON(CommonAssetType::LIGHT_DEF, ASSET_TYPE_LIGHT_DEF)
            MAP_COMMON(CommonAssetType::UI_MAP, ASSET_TYPE_UI_MAP)
            MAP_COMMON(CommonAssetType::FONT, ASSET_TYPE_FONT)
            MAP_COMMON(CommonAssetType::MENU_LIST, ASSET_TYPE_MENUFILE)
            MAP_COMMON(CommonAssetType::MENU, ASSET_TYPE_MENU)
            MAP_COMMON(CommonAssetType::LOCALIZE_ENTRY, ASSET_TYPE_LOCALIZE_ENTRY)
            MAP_COMMON(CommonAssetType::WEAPON, ASSET_TYPE_WEAPON)
            MAP_COMMON(CommonAssetType::SOUND_DRIVER_GLOBALS, ASSET_TYPE_SNDDRIVER_GLOBALS)
            MAP_COMMON(CommonAssetType::FX, ASSET_TYPE_FX)
            MAP_COMMON(CommonAssetType::IMPACT_FX, ASSET_TYPE_IMPACT_FX)
            MAP_COMMON(CommonAssetType::AI_TYPE, ASSET_TYPE_AITYPE)
            MAP_COMMON(CommonAssetType::MP_TYPE, ASSET_TYPE_MPTYPE)
            MAP_COMMON(CommonAssetType::CHARACTER, ASSET_TYPE_CHARACTER)
            MAP_COMMON(CommonAssetType::XMODEL_ALIAS, ASSET_TYPE_XMODELALIAS)
            MAP_COMMON(CommonAssetType::RAW_FILE, ASSET_TYPE_RAWFILE)
            MAP_COMMON(CommonAssetType::STRING_TABLE, ASSET_TYPE_STRINGTABLE)
            MAP_COMMON(CommonAssetType::XML_TREE, ASSET_TYPE_XMLTREE)
            MAP_COMMON(CommonAssetType::SCENE_ANIMATION, ASSET_TYPE_SCENE_ANIMATION)
            MAP_COMMON(CommonAssetType::CUTSCENE, ASSET_TYPE_CUTSCENE)
            MAP_COMMON(CommonAssetType::CUSTOM_CAMERA, ASSET_TYPE_CUSTOM_CAMERA)

        default:
            return std::nullopt;
        }

#undef MAP_COMMON
    }
} // namespace QOS
