#pragma once

#include "IGame.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <optional>

enum class CommonAssetType : std::uint8_t
{
    // IW3, IW4, IW5, T5, T6
    PHYS_PRESET,
    // IW3, IW4, IW5, T5, T6
    XANIM,
    // IW3, IW4, IW5, T5, T6
    XMODEL,
    // IW3, IW4, IW5, T5, T6
    MATERIAL,
    // IW3, IW4, IW5, T5, T6
    TECHNIQUE_SET,
    // IW3, IW4, IW5, T5, T6
    IMAGE,
    // IW3, IW4, IW5, T5, T6
    SOUND,
    // IW3, IW4, IW5
    SOUND_CURVE,
    // IW3, IW4, IW5
    LOADED_SOUND,
    // IW3, IW4, IW5, T5, T6
    CLIP_MAP,
    // IW3, IW4, IW5, T5, T6
    COM_WORLD,
    // IW3, IW4, T5, T6
    GAME_WORLD_SP,
    // IW3, IW4, T5, T6
    GAME_WORLD_MP,
    // IW3, IW4, IW5, T5, T6
    MAP_ENTS,
    // IW3, IW4, IW5, T5, T6
    GFX_WORLD,
    // IW3, IW4, IW5, T5, T6
    LIGHT_DEF,
    // IW3, IW4, IW5, T5, T6
    UI_MAP,
    // IW3, IW4, IW5, T5, T6
    FONT,
    // IW3, IW4, IW5, T5, T6
    MENU_LIST,
    // IW3, IW4, IW5, T5, T6
    MENU,
    // IW3, IW4, IW5, T5, T6
    LOCALIZE_ENTRY,
    // IW3, IW4, IW5, T5, T6
    WEAPON,
    // IW3, IW4, IW5, T5, T6
    SOUND_DRIVER_GLOBALS,
    // IW3, IW4, IW5, T5, T6
    FX,
    // IW3, IW4, IW5, T5, T6
    IMPACT_FX,
    // IW3, IW4, IW5, T5, T6
    AI_TYPE,
    // IW3, IW4, IW5, T5, T6
    MP_TYPE,
    // IW3, IW4, IW5, T5, T6
    CHARACTER,
    // IW3, IW4, IW5, T5, T6
    XMODEL_ALIAS,
    // IW3, IW4, IW5, T5, T6
    RAW_FILE,
    // IW3, IW4, IW5, T5, T6
    STRING_TABLE,

    // IW3, T5, T6
    XMODEL_PIECES,

    // IW4, IW5
    PHYS_COLL_MAP,
    // IW4, IW5
    XMODEL_SURFS,
    // IW4, IW5
    PIXEL_SHADER,
    // IW4, IW5
    VERTEX_SHADER,
    // IW4, IW5
    VERTEX_DECL,
    // IW4, IW5
    FX_WORLD,
    // IW4, IW5, T6
    LEADERBOARD,
    // IW4, IW5
    STRUCTURED_DATA_DEF,
    // IW4, IW5, T6
    TRACER,
    // IW4, IW5, T6
    VEHICLE,
    // IW4, IW5, T6
    ADDON_MAP_ENTS,

    // IW5
    GLASS_WORLD,
    // IW5
    PATH_DATA,
    // IW5
    VEHICLE_TRACK,
    // IW5, T6
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
