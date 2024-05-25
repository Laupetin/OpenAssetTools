#pragma once

// clang-format off: Order of includes matters here

// #include <d3d9.h>
#include "Game/IAsset.h"
#include "Image/Texture.h"

#include "IW5_Assets.h"

// clang-format on

namespace IW5
{
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
        char magic[8];                        // + 0
        unsigned int reserved;                // + 8
        DB_AuthHash subheaderHash;            // + 12
        DB_AuthSignature signedSubheaderHash; // + 44
        DB_AuthSubHeader subheader;           // + 300
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
        CSPFT_BOOL,
        CSPFT_FLOAT,
        CSPFT_MPH_TO_INCHES_PER_SEC,
        CSPFT_MILLISECONDS,
        CSPFT_FX,
        CSPFT_XMODEL,
        CSPFT_MATERIAL,
        CSPFT_PHYS_COLLMAP,
        CSPFT_SOUND,
        CSPFT_TRACER,
        CSPFT_SCRIPT_STRING,

        CSPFT_NUM_BASE_FIELD_TYPES,
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
        WFT_PER_SURFACE_TYPE_SOUND,
        WFT_STICKINESS,
        WFT_OVERLAYINTERFACE,
        WFT_INVENTORYTYPE,
        WFT_FIRETYPE,
        WFT_AMMOCOUNTER_CLIPTYPE,
        WFT_ICONRATIO_HUD,
        WFT_ICONRATIO_PICKUP,
        WFT_ICONRATIO_AMMOCOUNTER,
        WFT_ICONRATIO_KILL,
        WFT_ICONRATIO_DPAD,
        WFT_HIDETAGS,
        WFT_NOTETRACKSOUNDMAP,
        WFT_NOTETRACKRUMBLEMAP,

        // Custom
        WFT_ANIM_NAME,
        WFT_ATTACHMENT,
        WFT_ANIM_OVERRIDES,
        WFT_SOUND_OVERRIDES,
        WFT_FX_OVERRIDES,
        WFT_RELOAD_OVERRIDES,
        WFT_NOTETRACK_OVERRIDES,

        WFT_NUM_FIELD_TYPES,
    };

    enum weaponAttachmentFieldType_t
    {
        WAFT_ATTACHMENT_TYPE = CSPFT_NUM_BASE_FIELD_TYPES,
        WAFT_WEAPONTYPE,
        WAFT_WEAPONCLASS,
        WAFT_PENETRATE_TYPE,
        WAFT_IMPACT_TYPE,
        WAFT_FIRETYPE,

        WAFT_NUM_FIELD_TYPES,
    };

    using AssetPhysPreset = Asset<ASSET_TYPE_PHYSPRESET, PhysPreset>;
    using AssetPhysCollMap = Asset<ASSET_TYPE_PHYSCOLLMAP, PhysCollmap>;
    using AssetXAnim = Asset<ASSET_TYPE_XANIMPARTS, XAnimParts>;
    using AssetXModelSurfs = Asset<ASSET_TYPE_XMODEL_SURFS, XModelSurfs>;
    using AssetXModel = Asset<ASSET_TYPE_XMODEL, XModel>;
    using AssetMaterial = Asset<ASSET_TYPE_MATERIAL, Material>;
    using AssetPixelShader = Asset<ASSET_TYPE_PIXELSHADER, MaterialPixelShader>;
    using AssetVertexShader = Asset<ASSET_TYPE_VERTEXSHADER, MaterialVertexShader>;
    using AssetVertexDecl = Asset<ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration>;
    using AssetTechniqueSet = Asset<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>;
    using AssetImage = Asset<ASSET_TYPE_IMAGE, GfxImage>;
    using AssetSound = Asset<ASSET_TYPE_SOUND, snd_alias_list_t>;
    using AssetSoundCurve = Asset<ASSET_TYPE_SOUND_CURVE, SndCurve>;
    using AssetLoadedSound = Asset<ASSET_TYPE_LOADED_SOUND, LoadedSound>;
    using AssetClipMap = Asset<ASSET_TYPE_CLIPMAP, clipMap_t>;
    using AssetComWorld = Asset<ASSET_TYPE_COMWORLD, ComWorld>;
    using AssetGlassWorld = Asset<ASSET_TYPE_GLASSWORLD, GlassWorld>;
    using AssetPathData = Asset<ASSET_TYPE_PATHDATA, PathData>;
    using AssetVehicleTrack = Asset<ASSET_TYPE_VEHICLE_TRACK, VehicleTrack>;
    using AssetMapEnts = Asset<ASSET_TYPE_MAP_ENTS, MapEnts>;
    using AssetFxWorld = Asset<ASSET_TYPE_FXWORLD, FxWorld>;
    using AssetGfxWorld = Asset<ASSET_TYPE_GFXWORLD, GfxWorld>;
    using AssetLightDef = Asset<ASSET_TYPE_LIGHT_DEF, GfxLightDef>;
    using AssetFont = Asset<ASSET_TYPE_FONT, Font_s>;
    using AssetMenuList = Asset<ASSET_TYPE_MENULIST, MenuList>;
    using AssetMenu = Asset<ASSET_TYPE_MENU, menuDef_t>;
    using AssetLocalize = Asset<ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry>;
    using AssetAttachment = Asset<ASSET_TYPE_ATTACHMENT, WeaponAttachment>;
    using AssetWeapon = Asset<ASSET_TYPE_WEAPON, WeaponCompleteDef>;
    using AssetFx = Asset<ASSET_TYPE_FX, FxEffectDef>;
    using AssetImpactFx = Asset<ASSET_TYPE_IMPACT_FX, FxImpactTable>;
    using AssetSurfaceFx = Asset<ASSET_TYPE_SURFACE_FX, SurfaceFxTable>;
    using AssetRawFile = Asset<ASSET_TYPE_RAWFILE, RawFile>;
    using AssetScript = Asset<ASSET_TYPE_SCRIPTFILE, ScriptFile>;
    using AssetStringTable = Asset<ASSET_TYPE_STRINGTABLE, StringTable>;
    using AssetLeaderboard = Asset<ASSET_TYPE_LEADERBOARD, LeaderboardDef>;
    using AssetStructuredDataDef = Asset<ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet>;
    using AssetTracer = Asset<ASSET_TYPE_TRACER, TracerDef>;
    using AssetVehicle = Asset<ASSET_TYPE_VEHICLE, VehicleDef>;
    using AssetAddonMapEnts = Asset<ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts>;
} // namespace IW5
