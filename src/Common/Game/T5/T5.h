#pragma once

//#include <d3d9.h>
#include "Image/Texture.h"

#include "T5_Assets.h"

namespace T5
{
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
        CSPFT_STRING,
        CSPFT_STRING_MAX_STRING_CHARS,
        CSPFT_STRING_MAX_QPATH,
        CSPFT_STRING_MAX_OSPATH,
        CSPFT_INT,
        CSPFT_BOOL,
        CSPFT_QBOOLEAN,
        CSPFT_FLOAT,
        CSPFT_MILLISECONDS,
        CSPFT_FX,
        CSPFT_XMODEL,
        CSPFT_MATERIAL,
        CSPFT_MATERIAL_STREAM,
        CSPFT_PHYS_PRESET,
        CSPFT_SCRIPT_STRING,

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
        WFT_OFFHAND_SLOT,
        WFT_ANIMTYPE,
        WFT_ACTIVE_RETICLE_TYPE,
        WFT_GUIDED_MISSILE_TYPE,
        WFT_BOUNCE_SOUND,
        WFT_STICKINESS,
        WFT_ROTATETYPE,
        WFT_OVERLAYINTERFACE,
        WFT_INVENTORYTYPE,
        WFT_FIRETYPE,
        WFT_CLIPTYPE,
        WFT_AMMOCOUNTER_CLIPTYPE,
        WFT_ICONRATIO_HUD,
        WFT_ICONRATIO_AMMOCOUNTER,
        WFT_ICONRATIO_KILL,
        WFT_ICONRATIO_DPAD,
        WFT_ICONRATIO_INDICATOR,
        WFT_HIDETAGS,
        WFT_EXPLOSION_TAG,
        WFT_NOTETRACKSOUNDMAP,

        WFT_NUM_FIELD_TYPES
    };

    enum VehicleFieldType
    {
        VFT_TYPE = CSPFT_NUM_BASE_FIELD_TYPES,
        VFT_CAMERAMODE,
        VFT_BOOSTMODE,
        VFT_TRACTION_TYPE,
        VFT_MPH_TO_INCHES_PER_SECOND,
        VFT_POUNDS_TO_GAME_MASS,
        VFT_SCR_STRING,
        VFT_TEAM,
        VFT_KEY_BINDING,
        VFT_GRAPH,

        VFT_NUM
    };

    enum constraintsFieldType_t
    {
        CFT_TYPE = CSPFT_NUM_BASE_FIELD_TYPES,

        CFT_NUM_FIELD_TYPES
    };
}
