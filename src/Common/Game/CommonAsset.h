#pragma once

#include "IGame.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <optional>

enum class CommonAssetType : std::uint8_t
{
    // IW3, IW4, IW5, T5, T6, CODO
    PHYS_PRESET,
    // IW3, IW4, IW5, T5, T6, CODO
    XANIM,
    // IW3, IW4, IW5, T5, T6, CODO
    XMODEL,
    // IW3, IW4, IW5, T5, T6, CODO
    MATERIAL,
    // IW3, IW4, IW5, T5, T6, CODO
    TECHNIQUE_SET,
    // IW3, IW4, IW5, T5, T6, CODO
    IMAGE,
    // IW3, IW4, IW5, T5, T6, CODO
    SOUND,
    // IW3, IW4, IW5, CODO
    SOUND_CURVE,
    // IW3, IW4, IW5, CODO
    LOADED_SOUND,
    // IW3, IW4, IW5, T5, T6, CODO
    CLIP_MAP,
    // IW3, IW4, IW5, T5, T6, CODO
    COM_WORLD,
    // IW3, IW4, T5, T6
    GAME_WORLD_SP,
    // IW3, IW4, T5, T6
    GAME_WORLD_MP,
    // IW3, IW4, IW5, T5, T6, CODO
    MAP_ENTS,
    // IW3, IW4, IW5, T5, T6, CODO
    GFX_WORLD,
    // IW3, IW4, IW5, T5, T6, CODO
    LIGHT_DEF,
    // IW3, IW4, IW5, T5, T6, CODO
    UI_MAP,
    // IW3, IW4, IW5, T5, T6, CODO
    FONT,
    // IW3, IW4, IW5, T5, T6, CODO
    MENU_LIST,
    // IW3, IW4, IW5, T5, T6, CODO
    MENU,
    // IW3, IW4, IW5, T5, T6, CODO
    LOCALIZE_ENTRY,
    // IW3, IW4, IW5, T5, T6, CODO
    WEAPON,
    // IW3, IW4, IW5, T5, T6, CODO
    SOUND_DRIVER_GLOBALS,
    // IW3, IW4, IW5, T5, T6, CODO
    FX,
    // IW3, IW4, IW5, T5, T6, CODO
    IMPACT_FX,
    // IW3, IW4, IW5, T5, T6, CODO
    AI_TYPE,
    // IW3, IW4, IW5, T5, T6, CODO
    MP_TYPE,
    // IW3, IW4, IW5, T5, T6, CODO
    CHARACTER,
    // IW3, IW4, IW5, T5, T6, CODO
    XMODEL_ALIAS,
    // IW3, IW4, IW5, T5, T6, CODO
    RAW_FILE,
    // IW3, IW4, IW5, T5, T6, CODO
    STRING_TABLE,

    // IW3, T5, T6
    XMODEL_PIECES,

    // IW4, IW5, CODO
    PHYS_COLL_MAP,
    // IW4, IW5, CODO
    XMODEL_SURFS,
    // IW4, IW5, CODO
    PIXEL_SHADER,
    // IW4, IW5, CODO
    VERTEX_SHADER,
    // IW4, IW5, CODO
    VERTEX_DECL,
    // IW4, IW5, CODO
    FX_WORLD,
    // IW4, IW5, T6, CODO
    LEADERBOARD,
    // IW4, IW5, CODO
    STRUCTURED_DATA_DEF,
    // IW4, IW5, T6, CODO
    TRACER,
    // IW4, IW5, T6, CODO
    VEHICLE,
    // IW4, IW5, T6, CODO
    ADDON_MAP_ENTS,

    // IW5, CODO
    GLASS_WORLD,
    // IW5, CODO
    PATH_DATA,
    // IW5, CODO
    VEHICLE_TRACK,
    // IW5, T6, CODO
    ATTACHMENT,
    // IW5
    SURFACE_FX,
    // IW5, T6
    SCRIPT,

    // T5, T6
    PHYS_CONSTRAINTS,
    // T5, T6
    DESTRUCTIBLE_DEF,
    // T5, T6
    SOUND_PATCH,
    // T5, T6
    WEAPON_DEF,
    // T5, T6
    WEAPON_VARIANT,
    // T5, T6
    MP_BODY,
    // T5, T6
    MP_HEAD,
    // T5
    PACK_INDEX,
    // T5, T6
    XGLOBALS,
    // T5, T6
    DDL,
    // T5, T6
    GLASSES,
    // T5, T6
    EMBLEM_SET,

    // T6
    FONT_ICON,
    // T6
    WEAPON_FULL,
    // T6
    ATTACHMENT_UNIQUE,
    // T6
    WEAPON_CAMO,
    // T6
    KEY_VALUE_PAIRS,
    // T6
    MEMORY_BLOCK,
    // T6
    SKINNED_VERTS,
    // T6
    QDB,
    // T6
    SLUG,
    // T6
    FOOTSTEP_TABLE,
    // T6
    FOOTSTEP_FX_TABLE,
    // T6
    ZBARRIER,

    // QOS
    XML_TREE,
    SCENE_ANIMATION,
    CUTSCENE,
    CUSTOM_CAMERA,

    // CODO
    NET_CONST_STRINGS,
    MARKETPLACE_DATA,

    COUNT,
};

class ICommonAssetTypeMapper
{
public:
    [[nodiscard]] virtual CommonAssetType GameToCommonAssetType(asset_type_t gameAssetType) const = 0;
    [[nodiscard]] virtual std::optional<asset_type_t> CommonToGameAssetType(CommonAssetType commonAssetType) const = 0;

    static ICommonAssetTypeMapper* GetCommonAssetMapperByGame(GameId gameId);

protected:
    ICommonAssetTypeMapper() = default;
    virtual ~ICommonAssetTypeMapper() = default;
    ICommonAssetTypeMapper(const ICommonAssetTypeMapper& other) = default;
    ICommonAssetTypeMapper(ICommonAssetTypeMapper&& other) noexcept = default;
    ICommonAssetTypeMapper& operator=(const ICommonAssetTypeMapper& other) = default;
    ICommonAssetTypeMapper& operator=(ICommonAssetTypeMapper&& other) noexcept = default;
};
