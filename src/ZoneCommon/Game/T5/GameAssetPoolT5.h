#pragma once

#include "Game/T5/T5.h"
#include "Pool/AssetPool.h"
#include "Pool/ZoneAssetPools.h"

#include <memory>

class GameAssetPoolT5 final : public ZoneAssetPools
{
public:
    std::unique_ptr<AssetPool<T5::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<T5::PhysConstraints>> m_phys_constraints;
    std::unique_ptr<AssetPool<T5::DestructibleDef>> m_destructible_def;
    std::unique_ptr<AssetPool<T5::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<T5::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<T5::Material>> m_material;
    std::unique_ptr<AssetPool<T5::MaterialTechniqueSet>> m_technique_set;
    std::unique_ptr<AssetPool<T5::GfxImage>> m_image;
    std::unique_ptr<AssetPool<T5::SndBank>> m_sound_bank;
    std::unique_ptr<AssetPool<T5::SndPatch>> m_sound_patch;
    std::unique_ptr<AssetPool<T5::clipMap_t>> m_clip_map;
    std::unique_ptr<AssetPool<T5::ComWorld>> m_com_world;
    std::unique_ptr<AssetPool<T5::GameWorldSp>> m_game_world_sp;
    std::unique_ptr<AssetPool<T5::GameWorldMp>> m_game_world_mp;
    std::unique_ptr<AssetPool<T5::MapEnts>> m_map_ents;
    std::unique_ptr<AssetPool<T5::GfxWorld>> m_gfx_world;
    std::unique_ptr<AssetPool<T5::GfxLightDef>> m_gfx_light_def;
    std::unique_ptr<AssetPool<T5::Font_s>> m_font;
    std::unique_ptr<AssetPool<T5::MenuList>> m_menu_list;
    std::unique_ptr<AssetPool<T5::menuDef_t>> m_menu_def;
    std::unique_ptr<AssetPool<T5::LocalizeEntry>> m_localize;
    std::unique_ptr<AssetPool<T5::WeaponVariantDef>> m_weapon;
    std::unique_ptr<AssetPool<T5::SndDriverGlobals>> m_snd_driver_globals;
    std::unique_ptr<AssetPool<T5::FxEffectDef>> m_fx;
    std::unique_ptr<AssetPool<T5::FxImpactTable>> m_fx_impact_table;
    std::unique_ptr<AssetPool<T5::RawFile>> m_raw_file;
    std::unique_ptr<AssetPool<T5::StringTable>> m_string_table;
    std::unique_ptr<AssetPool<T5::PackIndex>> m_pack_index;
    std::unique_ptr<AssetPool<T5::XGlobals>> m_xglobals;
    std::unique_ptr<AssetPool<T5::ddlRoot_t>> m_ddl;
    std::unique_ptr<AssetPool<T5::Glasses>> m_glasses;
    std::unique_ptr<AssetPool<T5::EmblemSet>> m_emblem_set;

    GameAssetPoolT5(Zone* zone, zone_priority_t priority);
    ~GameAssetPoolT5() override = default;

    [[nodiscard]] XAssetInfoGeneric* GetAsset(asset_type_t type, const std::string& name) const override;

    static std::optional<const char*> AssetTypeNameByType(asset_type_t assetType);
    [[nodiscard]] std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const override;

    static asset_type_t AssetTypeCount();
    [[nodiscard]] asset_type_t GetAssetTypeCount() const override;

protected:
    XAssetInfoGeneric* AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;

private:
    zone_priority_t m_priority;
};
