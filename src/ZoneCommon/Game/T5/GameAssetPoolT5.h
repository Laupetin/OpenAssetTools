#pragma once

#include "Pool/ZoneAssetPools.h"
#include "Pool/AssetPool.h"
#include "Game/T5/T5.h"

class GameAssetPoolT5 final : public ZoneAssetPools
{
    int m_priority;

    static constexpr const char* ASSET_TYPE_INVALID = "invalid_asset_type";
    static const char* ASSET_TYPE_NAMES[];

protected:
    XAssetInfoGeneric* AddAssetToPool(asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings, Zone* zone) override;

public:
    AssetPool<T5::PhysPreset>* m_phys_preset;
    AssetPool<T5::PhysConstraints>* m_phys_constraints;
    AssetPool<T5::DestructibleDef>* m_destructible_def;
    AssetPool<T5::XAnimParts>* m_xanim_parts;
    AssetPool<T5::XModel>* m_xmodel;
    AssetPool<T5::Material>* m_material;
    AssetPool<T5::MaterialTechniqueSet>* m_technique_set;
    AssetPool<T5::GfxImage>* m_image;
    AssetPool<T5::SndBank>* m_sound_bank;
    AssetPool<T5::SndPatch>* m_sound_patch;
    AssetPool<T5::clipMap_t>* m_clip_map;
    AssetPool<T5::ComWorld>* m_com_world;
    AssetPool<T5::GameWorldSp>* m_game_world_sp;
    AssetPool<T5::GameWorldMp>* m_game_world_mp;
    AssetPool<T5::MapEnts>* m_map_ents;
    AssetPool<T5::GfxWorld>* m_gfx_world;
    AssetPool<T5::GfxLightDef>* m_gfx_light_def;
    AssetPool<T5::Font_s>* m_font;
    AssetPool<T5::MenuList>* m_menu_list;
    AssetPool<T5::menuDef_t>* m_menu_def;
    AssetPool<T5::LocalizeEntry>* m_localize;
    AssetPool<T5::WeaponVariantDef>* m_weapon;
    AssetPool<T5::SndDriverGlobals>* m_snd_driver_globals;
    AssetPool<T5::FxEffectDef>* m_fx;
    AssetPool<T5::FxImpactTable>* m_fx_impact_table;
    AssetPool<T5::RawFile>* m_raw_file;
    AssetPool<T5::StringTable>* m_string_table;
    AssetPool<T5::PackIndex>* m_pack_index;
    AssetPool<T5::XGlobals>* m_xglobals;
    AssetPool<T5::ddlRoot_t>* m_ddl;
    AssetPool<T5::Glasses>* m_glasses;
    AssetPool<T5::EmblemSet>* m_emblem_set;

    GameAssetPoolT5(Zone* zone, int priority);
    ~GameAssetPoolT5() override;

    void InitPoolStatic(asset_type_t type, size_t capacity) override;
    void InitPoolDynamic(asset_type_t type) override;

    XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const override;
    static const char* AssetTypeNameByType(asset_type_t assetType);
    const char* GetAssetTypeName(asset_type_t assetType) const override;
};
