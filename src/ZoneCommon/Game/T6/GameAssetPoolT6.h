#pragma once

#include "Game/T6/T6.h"
#include "Pool/AssetPool.h"
#include "Pool/ZoneAssetPools.h"

#include <memory>

class GameAssetPoolT6 final : public ZoneAssetPools
{
    int m_priority;

    static constexpr const char* ASSET_TYPE_INVALID = "invalid_asset_type";
    static const char* ASSET_TYPE_NAMES[];

protected:
    XAssetInfoGeneric* AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;

public:
    std::unique_ptr<AssetPool<T6::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<T6::PhysConstraints>> m_phys_constraints;
    std::unique_ptr<AssetPool<T6::DestructibleDef>> m_destructible_def;
    std::unique_ptr<AssetPool<T6::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<T6::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<T6::Material>> m_material;
    std::unique_ptr<AssetPool<T6::MaterialTechniqueSet>> m_technique_set;
    std::unique_ptr<AssetPool<T6::GfxImage>> m_image;
    std::unique_ptr<AssetPool<T6::SndBank>> m_sound_bank;
    std::unique_ptr<AssetPool<T6::SndPatch>> m_sound_patch;
    std::unique_ptr<AssetPool<T6::clipMap_t>> m_clip_map;
    std::unique_ptr<AssetPool<T6::ComWorld>> m_com_world;
    std::unique_ptr<AssetPool<T6::GameWorldSp>> m_game_world_sp;
    std::unique_ptr<AssetPool<T6::GameWorldMp>> m_game_world_mp;
    std::unique_ptr<AssetPool<T6::MapEnts>> m_map_ents;
    std::unique_ptr<AssetPool<T6::GfxWorld>> m_gfx_world;
    std::unique_ptr<AssetPool<T6::GfxLightDef>> m_gfx_light_def;
    std::unique_ptr<AssetPool<T6::Font_s>> m_font;
    std::unique_ptr<AssetPool<T6::FontIcon>> m_font_icon;
    std::unique_ptr<AssetPool<T6::MenuList>> m_menu_list;
    std::unique_ptr<AssetPool<T6::menuDef_t>> m_menu_def;
    std::unique_ptr<AssetPool<T6::LocalizeEntry>> m_localize;
    std::unique_ptr<AssetPool<T6::WeaponVariantDef>> m_weapon;
    std::unique_ptr<AssetPool<T6::WeaponAttachment>> m_attachment;
    std::unique_ptr<AssetPool<T6::WeaponAttachmentUnique>> m_attachment_unique;
    std::unique_ptr<AssetPool<T6::WeaponCamo>> m_camo;
    std::unique_ptr<AssetPool<T6::SndDriverGlobals>> m_snd_driver_globals;
    std::unique_ptr<AssetPool<T6::FxEffectDef>> m_fx;
    std::unique_ptr<AssetPool<T6::FxImpactTable>> m_fx_impact_table;
    std::unique_ptr<AssetPool<T6::RawFile>> m_raw_file;
    std::unique_ptr<AssetPool<T6::StringTable>> m_string_table;
    std::unique_ptr<AssetPool<T6::LeaderboardDef>> m_leaderboard;
    std::unique_ptr<AssetPool<T6::XGlobals>> m_xglobals;
    std::unique_ptr<AssetPool<T6::ddlRoot_t>> m_ddl;
    std::unique_ptr<AssetPool<T6::Glasses>> m_glasses;
    std::unique_ptr<AssetPool<T6::EmblemSet>> m_emblem_set;
    std::unique_ptr<AssetPool<T6::ScriptParseTree>> m_script;
    std::unique_ptr<AssetPool<T6::KeyValuePairs>> m_key_value_pairs;
    std::unique_ptr<AssetPool<T6::VehicleDef>> m_vehicle;
    std::unique_ptr<AssetPool<T6::MemoryBlock>> m_memory_block;
    std::unique_ptr<AssetPool<T6::AddonMapEnts>> m_addon_map_ents;
    std::unique_ptr<AssetPool<T6::TracerDef>> m_tracer;
    std::unique_ptr<AssetPool<T6::SkinnedVertsDef>> m_skinned_verts;
    std::unique_ptr<AssetPool<T6::Qdb>> m_qdb;
    std::unique_ptr<AssetPool<T6::Slug>> m_slug;
    std::unique_ptr<AssetPool<T6::FootstepTableDef>> m_footstep_table;
    std::unique_ptr<AssetPool<T6::FootstepFXTableDef>> m_footstep_fx_table;
    std::unique_ptr<AssetPool<T6::ZBarrierDef>> m_zbarrier;

    GameAssetPoolT6(Zone* zone, int priority);
    ~GameAssetPoolT6() override = default;

    void InitPoolStatic(asset_type_t type, size_t capacity) override;
    void InitPoolDynamic(asset_type_t type) override;

    _NODISCARD XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const override;

    static const char* AssetTypeNameByType(asset_type_t assetType);
    _NODISCARD const char* GetAssetTypeName(asset_type_t assetType) const override;

    static asset_type_t AssetTypeCount();
    _NODISCARD asset_type_t GetAssetTypeCount() const override;
};
