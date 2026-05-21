#pragma once

// clang-format off: Order of includes matters here

// #include <d3d9.h>
#include "Game/IAsset.h"

#include "IW6_Assets.h"

// clang-format on

namespace IW6
{
    enum XAssetType
    {
        ASSET_TYPE_PHYSPRESET,
        ASSET_TYPE_PHYSCOLLMAP,
        ASSET_TYPE_XANIMPARTS,
        ASSET_TYPE_XMODEL_SURFS,
        ASSET_TYPE_XMODEL,
        ASSET_TYPE_MATERIAL,
        ASSET_TYPE_COMPUTESHADER,
        ASSET_TYPE_VERTEXSHADER,
        ASSET_TYPE_HULLSHADER,
        ASSET_TYPE_DOMAINSHADER,
        ASSET_TYPE_PIXELSHADER,
        ASSET_TYPE_VERTEXDECL,
        ASSET_TYPE_TECHNIQUE_SET,
        ASSET_TYPE_IMAGE,
        ASSET_TYPE_SOUND,
        ASSET_TYPE_SOUND_CURVE,
        ASSET_TYPE_LPF_CURVE,
        ASSET_TYPE_REVERB_CURVE,
        ASSET_TYPE_LOADED_SOUND,
        ASSET_TYPE_CLIPMAP,
        ASSET_TYPE_COMWORLD,
        ASSET_TYPE_GLASSWORLD,
        ASSET_TYPE_PATHDATA,
        ASSET_TYPE_VEHICLE_TRACK,
        ASSET_TYPE_MAP_ENTS,
        ASSET_TYPE_FXWORLD,
        ASSET_TYPE_GFXWORLD,
        ASSET_TYPE_LIGHT_DEF,
        ASSET_TYPE_UI_MAP,
        ASSET_TYPE_FONT,
        ASSET_TYPE_MENULIST,
        ASSET_TYPE_MENU,
        ASSET_TYPE_ANIMCLASS,
        ASSET_TYPE_LOCALIZE_ENTRY,
        ASSET_TYPE_ATTACHMENT,
        ASSET_TYPE_WEAPON,
        ASSET_TYPE_SNDDRIVER_GLOBALS,
        ASSET_TYPE_FX,
        ASSET_TYPE_IMPACT_FX,
        ASSET_TYPE_SURFACE_FX,
        ASSET_TYPE_AITYPE,
        ASSET_TYPE_MPTYPE,
        ASSET_TYPE_CHARACTER,
        ASSET_TYPE_XMODELALIAS,
        ASSET_TYPE_RAWFILE,
        ASSET_TYPE_SCRIPTFILE,
        ASSET_TYPE_STRINGTABLE,
        ASSET_TYPE_LEADERBOARD,
        ASSET_TYPE_STRUCTURED_DATA_DEF,
        ASSET_TYPE_TRACER,
        ASSET_TYPE_VEHICLE,
        ASSET_TYPE_ADDON_MAP_ENTS,
        ASSET_TYPE_NET_CONST_STRINGS,
        ASSET_TYPE_REVERB_PRESET,
        ASSET_TYPE_LUA_FILE,
        ASSET_TYPE_SCRIPTABLE,
        ASSET_TYPE_COLORIZATION,
        ASSET_TYPE_COLORIZATIONSET,
        ASSET_TYPE_TONEMAPPING,
        ASSET_TYPE_EQUIPMENT_SND_TABLE,
        ASSET_TYPE_VECTORFIELD,
        ASSET_TYPE_DOPPLER_PRESET,
        ASSET_TYPE_PARTICLE_SIM_ANIMATION,

        ASSET_TYPE_COUNT,
    };

    enum SubAssetType
    {
        SUB_ASSET_TYPE_TECHNIQUE,

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
        char magic[8];                        // + 0
        unsigned int reserved;                // + 8
        DB_AuthHash subheaderHash;            // + 12
        DB_AuthSignature signedSubheaderHash; // + 44
        DB_AuthSubHeader subheader;           // + 300
    };

    struct ScriptStringList
    {
        int64_t count;
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
        int64_t assetCount;
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
        CSPFT_SOUND,
        CSPFT_TRACER,
        CSPFT_NUM_BASE_FIELD_TYPES,
    };

    // TODO: Verify
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

    // TODO: Verify
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

    // TODO: Verify
    enum physPresetFieldType_t
    {
        PPFT_SCALING = CSPFT_NUM_BASE_FIELD_TYPES,

        PPFT_NUM_FIELD_TYPES,
    };

    using AssetPhysPreset = Asset<ASSET_TYPE_PHYSPRESET, PhysPreset>;
    using AssetPhysCollMap = Asset<ASSET_TYPE_PHYSCOLLMAP, PhysCollmap>;
    using AssetXAnim = Asset<ASSET_TYPE_XANIMPARTS, XAnimParts>;
    using AssetXModelSurfs = Asset<ASSET_TYPE_XMODEL_SURFS, XModelSurfs>;
    using AssetXModel = Asset<ASSET_TYPE_XMODEL, XModel>;
    using AssetMaterial = Asset<ASSET_TYPE_MATERIAL, Material>;
    using AssetComputeShader = Asset<ASSET_TYPE_COMPUTESHADER, ComputeShader>;
    using AssetVertexShader = Asset<ASSET_TYPE_VERTEXSHADER, MaterialVertexShader>;
    using AssetHullShader = Asset<ASSET_TYPE_HULLSHADER, MaterialHullShader>;
    using AssetDomainShader = Asset<ASSET_TYPE_DOMAINSHADER, MaterialDomainShader>;
    using AssetPixelShader = Asset<ASSET_TYPE_PIXELSHADER, MaterialPixelShader>;
    using AssetVertexDecl = Asset<ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration>;
    using AssetTechniqueSet = Asset<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>;
    using AssetImage = Asset<ASSET_TYPE_IMAGE, GfxImage>;
    using AssetSound = Asset<ASSET_TYPE_SOUND, snd_alias_list_t>;
    using AssetSoundCurve = Asset<ASSET_TYPE_SOUND_CURVE, SoundCurve>;
    using AssetLpfCurve = Asset<ASSET_TYPE_LPF_CURVE, LpfCurve>;
    using AssetReverbCurve = Asset<ASSET_TYPE_REVERB_CURVE, ReverbCurve>;
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
    using AssetAnimationClass = Asset<ASSET_TYPE_ANIMCLASS, AnimationClass>;
    using AssetLocalize = Asset<ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry>;
    using AssetAttachment = Asset<ASSET_TYPE_ATTACHMENT, WeaponAttachment>;
    using AssetWeapon = Asset<ASSET_TYPE_WEAPON, WeaponCompleteDef>;
    using AssetSoundDriverGlobals = Asset<ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals>;
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
    using AssetNetConstStrings = Asset<ASSET_TYPE_NET_CONST_STRINGS, NetConstStrings>;
    using AssetReverbPreset = Asset<ASSET_TYPE_REVERB_PRESET, ReverbPreset>;
    using AssetLuaFile = Asset<ASSET_TYPE_LUA_FILE, LuaFile>;
    using AssetScriptable = Asset<ASSET_TYPE_SCRIPTABLE, ScriptableDef>;
    using AssetColorization = Asset<ASSET_TYPE_COLORIZATION, Colorization>;
    using AssetColorizationSet = Asset<ASSET_TYPE_COLORIZATIONSET, ColorizationSet>;
    using AssetToneMapping = Asset<ASSET_TYPE_TONEMAPPING, ToneMapping>;
    using AssetEquipmentSoundTable = Asset<ASSET_TYPE_EQUIPMENT_SND_TABLE, EquipmentSoundTable>;
    using AssetVectorField= Asset<ASSET_TYPE_VECTORFIELD, VectorField>;
    using AssetDopplerPreset = Asset<ASSET_TYPE_DOPPLER_PRESET, DopplerPreset>;
    using AssetFxParticleSimAnimation = Asset<ASSET_TYPE_PARTICLE_SIM_ANIMATION, FxParticleSimAnimation>;

    using SubAssetTechnique = SubAsset<SUB_ASSET_TYPE_TECHNIQUE, MaterialTechnique>;
} // namespace IW6

DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetPhysPreset, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetPhysCollMap, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetXAnim, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetXModelSurfs, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetXModel, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetMaterial, info.name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetComputeShader, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetVertexShader, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetHullShader, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetDomainShader, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetPixelShader, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetVertexDecl, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetTechniqueSet, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetImage, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetSound, aliasName);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetSoundCurve, curve.filename);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLpfCurve, curve.filename);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetReverbCurve, curve.filename);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLoadedSound, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetClipMap, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetComWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetGlassWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetPathData, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetVehicleTrack, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetFxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetGfxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLightDef, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetFont, fontName);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetMenuList, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetMenu, window.name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetAnimationClass, className);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLocalize, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetAttachment, szInternalName);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetWeapon, szInternalName);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetSoundDriverGlobals, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetFx, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetImpactFx, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetSurfaceFx, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetRawFile, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetScript, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetStringTable, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLeaderboard, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetStructuredDataDef, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetTracer, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetVehicle, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetAddonMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetNetConstStrings, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetReverbPreset, p_name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetLuaFile, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetScriptable, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetColorization, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetColorizationSet, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetToneMapping, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetEquipmentSoundTable, szName);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetVectorField, name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetDopplerPreset, p_name);
DEFINE_ASSET_NAME_ACCESSOR(IW6::AssetFxParticleSimAnimation, name);
