#pragma once

// clang-format off: Order of includes matters here

#include "Game/IAsset.h"

#include "CODO_Assets.h"

// clang-format on

namespace CODO
{
    enum XAssetType
    {
        ASSET_TYPE_PHYSPRESET,
        ASSET_TYPE_PHYSCOLLMAP,
        ASSET_TYPE_XANIMPARTS,
        ASSET_TYPE_XMODEL_SURFS,
        ASSET_TYPE_XMODEL,
        ASSET_TYPE_MATERIAL,
        ASSET_TYPE_PIXELSHADER,
        ASSET_TYPE_VERTEXSHADER,
        ASSET_TYPE_VERTEXDECL,
        ASSET_TYPE_TECHNIQUE_SET,
        ASSET_TYPE_IMAGE,
        ASSET_TYPE_SOUND,
        ASSET_TYPE_SOUND_CURVE,
        ASSET_TYPE_LOADED_SOUND,
        ASSET_TYPE_AIPATHS,
        ASSET_TYPE_CLIPMAP,
        ASSET_TYPE_COMWORLD,
        ASSET_TYPE_GLASSWORLD,
        ASSET_TYPE_VEHICLE_TRACK,
        ASSET_TYPE_MAP_ENTS,
        ASSET_TYPE_FXWORLD,
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
        ASSET_TYPE_LEADERBOARD,
        ASSET_TYPE_STRUCTURED_DATA_DEF,
        ASSET_TYPE_TRACER,
        ASSET_TYPE_VEHICLE,
        ASSET_TYPE_ADDON_MAP_ENTS,
        ASSET_TYPE_WEAPON_ATTACH,
        ASSET_TYPE_NET_CONST_STRINGS,
        ASSET_TYPE_MARKETPLACE_DATA,

        ASSET_TYPE_COUNT
    };

    enum SubAssetType
    {
        SUB_ASSET_TYPE_TECHNIQUE,
        SUB_ASSET_TYPE_ACCURACY_GRAPH,

        SUB_ASSET_TYPE_COUNT
    };

    struct ScriptStringList
    {
        int count;
        const char** strings;
    };

    struct XAsset
    {
        XAssetType type;
        unsigned int dataOffset; // Offset of the asset's data in the zone stream
        XAssetHeader header;
        unsigned int dataSize; // Size of the asset's data in the zone stream
    };

    struct XAssetList
    {
        ScriptStringList stringList;
        int assetCount;
        XAsset* assets;
    };

    using AssetPhysPreset = Asset<ASSET_TYPE_PHYSPRESET, PhysPreset>;
    using AssetPhysCollMap = Asset<ASSET_TYPE_PHYSCOLLMAP, PhysCollmap>;
    using AssetXAnim = Asset<ASSET_TYPE_XANIMPARTS, XAnimParts>;
    using AssetXModelSurfs = Asset<ASSET_TYPE_XMODEL_SURFS, XModelSurfs>;
    using AssetXModel = Asset<ASSET_TYPE_XMODEL, XModel>;
    using AssetMaterial = Asset<ASSET_TYPE_MATERIAL, Material>;
    using AssetTechniqueSet = Asset<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>;
    using AssetPixelShader = Asset<ASSET_TYPE_PIXELSHADER, MaterialPixelShader>;
    using AssetVertexShader = Asset<ASSET_TYPE_VERTEXSHADER, MaterialVertexShader>;
    using AssetVertexDecl = Asset<ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration>;
    using SubAssetTechnique = SubAsset<SUB_ASSET_TYPE_TECHNIQUE, MaterialTechnique>;
    using AssetImage = Asset<ASSET_TYPE_IMAGE, GfxImage>;
    using AssetSound = Asset<ASSET_TYPE_SOUND, snd_alias_list_t>;
    using AssetSoundCurve = Asset<ASSET_TYPE_SOUND_CURVE, SndCurve>;
    using AssetLoadedSound = Asset<ASSET_TYPE_LOADED_SOUND, LoadedSound>;
    using AssetClipMap = Asset<ASSET_TYPE_CLIPMAP, clipMap_t>;
    using AssetComWorld = Asset<ASSET_TYPE_COMWORLD, ComWorld>;
    using AssetMapEnts = Asset<ASSET_TYPE_MAP_ENTS, MapEnts>;
    using AssetFxWorld = Asset<ASSET_TYPE_FXWORLD, FxWorld>;
    using AssetGfxWorld = Asset<ASSET_TYPE_GFXWORLD, GfxWorld>;
    using AssetLightDef = Asset<ASSET_TYPE_LIGHT_DEF, GfxLightDef>;
    using AssetFont = Asset<ASSET_TYPE_FONT, Font_s>;
    using AssetMenuList = Asset<ASSET_TYPE_MENULIST, MenuList>;
    using AssetMenu = Asset<ASSET_TYPE_MENU, menuDef_t>;
    using AssetWeapon = Asset<ASSET_TYPE_WEAPON, WeaponCompleteDef>;
    using AssetFx = Asset<ASSET_TYPE_FX, FxEffectDef>;
    using AssetImpactFx = Asset<ASSET_TYPE_IMPACT_FX, FxImpactTable>;
    using AssetLocalize = Asset<ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry>;
    using AssetRawFile = Asset<ASSET_TYPE_RAWFILE, RawFile>;
    using AssetStringTable = Asset<ASSET_TYPE_STRINGTABLE, StringTable>;
    using AssetLeaderboard = Asset<ASSET_TYPE_LEADERBOARD, LeaderboardDef>;
    using AssetStructuredDataDef = Asset<ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet>;
    using AssetTracer = Asset<ASSET_TYPE_TRACER, TracerDef>;
    using AssetVehicle = Asset<ASSET_TYPE_VEHICLE, VehicleDef>;
    using AssetAddonMapEnts = Asset<ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts>;
    using AssetWeaponAttach = Asset<ASSET_TYPE_WEAPON_ATTACH, WeaponAttach>;
    using AssetNetConstStrings = Asset<ASSET_TYPE_NET_CONST_STRINGS, NetConstStrings>;
    using AssetMarketplaceData = Asset<ASSET_TYPE_MARKETPLACE_DATA, MarketplaceData>;
} // namespace CODO

DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetPhysPreset, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetPhysCollMap, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetXAnim, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetXModelSurfs, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetXModel, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetMaterial, info.name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetTechniqueSet, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetPixelShader, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetVertexShader, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetVertexDecl, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetImage, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetSound, aliasName);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetSoundCurve, filename);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetLoadedSound, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetClipMap, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetComWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetFxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetGfxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetLightDef, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetFont, fontName);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetMenuList, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetMenu, window.name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetWeapon, szInternalName);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetFx, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetImpactFx, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetLocalize, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetRawFile, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetStringTable, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetLeaderboard, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetStructuredDataDef, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetTracer, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetVehicle, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetAddonMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetWeaponAttach, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetNetConstStrings, name);
DEFINE_ASSET_NAME_ACCESSOR(CODO::AssetMarketplaceData, name);
