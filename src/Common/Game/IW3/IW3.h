#pragma once

// clang-format off: Order of includes matters here

// #include <d3d9.h>
#include "Game/IAsset.h"

#include "IW3_Assets.h"

// clang-format on

namespace IW3
{
    enum XAssetType
    {
        ASSET_TYPE_XMODELPIECES,
        ASSET_TYPE_PHYSPRESET,
        ASSET_TYPE_XANIMPARTS,
        ASSET_TYPE_XMODEL,
        ASSET_TYPE_MATERIAL,
        ASSET_TYPE_TECHNIQUE_SET,
        ASSET_TYPE_IMAGE,
        ASSET_TYPE_SOUND,
        ASSET_TYPE_SOUND_CURVE,
        ASSET_TYPE_LOADED_SOUND,
        ASSET_TYPE_CLIPMAP,
        ASSET_TYPE_CLIPMAP_PVS,
        ASSET_TYPE_COMWORLD,
        ASSET_TYPE_GAMEWORLD_SP,
        ASSET_TYPE_GAMEWORLD_MP,
        ASSET_TYPE_MAP_ENTS,
        ASSET_TYPE_GFXWORLD,
        ASSET_TYPE_LIGHT_DEF,
        ASSET_TYPE_UI_MAP,
        ASSET_TYPE_FONT,
        ASSET_TYPE_MENULIST,
        ASSET_TYPE_MENU,
        ASSET_TYPE_LOCALIZE_ENTRY,
        ASSET_TYPE_WEAPON,
        ASSET_TYPE_SNDDRIVER_GLOBALS,
        ASSET_TYPE_FX,
        ASSET_TYPE_IMPACT_FX,
        ASSET_TYPE_AITYPE,
        ASSET_TYPE_MPTYPE,
        ASSET_TYPE_CHARACTER,
        ASSET_TYPE_XMODELALIAS,
        ASSET_TYPE_RAWFILE,
        ASSET_TYPE_STRINGTABLE,

        ASSET_TYPE_COUNT
    };

    enum SubAssetType
    {
        SUB_ASSET_TYPE_TECHNIQUE,
        SUB_ASSET_TYPE_VERTEX_DECL,
        SUB_ASSET_TYPE_VERTEX_SHADER,
        SUB_ASSET_TYPE_PIXEL_SHADER,

        SUB_ASSET_TYPE_COUNT
    };

    struct DB_AuthHash
    {
        char bytes[32];
    };

    struct DB_AuthSignature
    {
        char bytes[256];
    };

    struct DB_AuthSubHeader
    {
        char fastfileName[32];
        unsigned int reserved;
        DB_AuthHash masterBlockHashes[244];
    };

    struct DB_AuthHeader
    {
        char magic[8];
        unsigned int reserved;
        DB_AuthHash subheaderHash;
        DB_AuthSignature signedSubheaderHash;
        DB_AuthSubHeader subheader;
    };

    struct ScriptStringList
    {
        int count;
        const char** strings;
    };

    struct XAsset
    {
        XAssetType type;
        XAssetHeader header;
    };

    struct XAssetList
    {
        ScriptStringList stringList;
        int assetCount;
        XAsset* assets;
    };

    struct cspField_t
    {
        const char* szName;
        int iOffset;
        int iFieldType;
    };

    enum csParseFieldType_t
    {
        CSPFT_STRING = 0,
        CSPFT_STRING_MAX_STRING_CHARS,
        CSPFT_STRING_MAX_QPATH,
        CSPFT_STRING_MAX_OSPATH,
        CSPFT_INT,
        CSPFT_QBOOLEAN,
        CSPFT_FLOAT,
        CSPFT_MILLISECONDS,
        CSPFT_FX,
        CSPFT_XMODEL,
        CSPFT_MATERIAL,
        CSPFT_SOUND,

        CSPFT_NUM_BASE_FIELD_TYPES
    };

    enum weapFieldType_t
    {
        WFT_WEAPONTYPE = CSPFT_NUM_BASE_FIELD_TYPES,
        WFT_WEAPONCLASS,
        WFT_OVERLAYRETICLE,
        WFT_PENETRATE_TYPE,
        WFT_IMPACT_TYPE,
        WFT_STANCE,
        WFT_PROJ_EXPLOSION,
        WFT_OFFHAND_CLASS,
        WFT_ANIMTYPE,
        WFT_ACTIVE_RETICLE_TYPE,
        WFT_GUIDED_MISSILE_TYPE,
        WFT_BOUNCE_SOUND,
        WFT_STICKINESS,
        WFT_OVERLAYINTERFACE,
        WFT_INVENTORYTYPE,
        WFT_FIRETYPE,
        WFT_AMMOCOUNTER_CLIPTYPE,
        WFT_ICONRATIO_HUD,
        WFT_ICONRATIO_AMMOCOUNTER,
        WFT_ICONRATIO_KILL,
        WFT_ICONRATIO_DPAD,
        WFT_HIDETAGS,
        WFT_NOTETRACKSOUNDMAP,

        WFT_NUM_FIELD_TYPES
    };

    using AssetXModelPieces = Asset<ASSET_TYPE_XMODELPIECES, XModelPieces>;
    using AssetPhysPreset = Asset<ASSET_TYPE_PHYSPRESET, PhysPreset>;
    using AssetXAnim = Asset<ASSET_TYPE_XANIMPARTS, XAnimParts>;
    using AssetXModel = Asset<ASSET_TYPE_XMODEL, XModel>;
    using AssetMaterial = Asset<ASSET_TYPE_MATERIAL, Material>;
    using AssetTechniqueSet = Asset<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>;
    using AssetImage = Asset<ASSET_TYPE_IMAGE, GfxImage>;
    using AssetSound = Asset<ASSET_TYPE_SOUND, snd_alias_list_t>;
    using AssetSoundCurve = Asset<ASSET_TYPE_SOUND_CURVE, SndCurve>;
    using AssetLoadedSound = Asset<ASSET_TYPE_LOADED_SOUND, LoadedSound>;
    using AssetClipMap = Asset<ASSET_TYPE_CLIPMAP, clipMap_t>;
    using AssetClipMapPvs = Asset<ASSET_TYPE_CLIPMAP_PVS, clipMap_t>;
    using AssetComWorld = Asset<ASSET_TYPE_COMWORLD, ComWorld>;
    using AssetGameWorldSp = Asset<ASSET_TYPE_GAMEWORLD_SP, GameWorldSp>;
    using AssetGameWorldMp = Asset<ASSET_TYPE_GAMEWORLD_MP, GameWorldMp>;
    using AssetMapEnts = Asset<ASSET_TYPE_MAP_ENTS, MapEnts>;
    using AssetGfxWorld = Asset<ASSET_TYPE_GFXWORLD, GfxWorld>;
    using AssetLightDef = Asset<ASSET_TYPE_LIGHT_DEF, GfxLightDef>;
    using AssetFont = Asset<ASSET_TYPE_FONT, Font_s>;
    using AssetMenuList = Asset<ASSET_TYPE_MENULIST, MenuList>;
    using AssetMenu = Asset<ASSET_TYPE_MENU, menuDef_t>;
    using AssetLocalize = Asset<ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry>;
    using AssetWeapon = Asset<ASSET_TYPE_WEAPON, WeaponDef>;
    using AssetSoundDriverGlobals = Asset<ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals>;
    using AssetFx = Asset<ASSET_TYPE_FX, FxEffectDef>;
    using AssetImpactFx = Asset<ASSET_TYPE_IMPACT_FX, FxImpactTable>;
    using AssetRawFile = Asset<ASSET_TYPE_RAWFILE, RawFile>;
    using AssetStringTable = Asset<ASSET_TYPE_STRINGTABLE, StringTable>;
} // namespace IW3

DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetXModelPieces, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetPhysPreset, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetXAnim, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetXModel, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetMaterial, info.name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetTechniqueSet, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetImage, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetSound, aliasName);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetSoundCurve, filename);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetLoadedSound, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetClipMap, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetClipMapPvs, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetComWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetGameWorldSp, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetGameWorldMp, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetGfxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetLightDef, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetFont, fontName);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetMenuList, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetMenu, window.name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetLocalize, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetWeapon, szInternalName);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetSoundDriverGlobals, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetFx, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetImpactFx, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetRawFile, name);
DEFINE_ASSET_NAME_ACCESSOR(IW3::AssetStringTable, name);
