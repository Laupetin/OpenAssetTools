#pragma once

#include <memory>

#include "Pool/IZoneAssetPools.h"
#include "Pool/AssetPool.h"
#include "IW4.h"

class GameAssetPoolIW4 final : public IZoneAssetPools
{
    int m_priority;
    std::vector<XAssetInfoGeneric*> m_assets_in_order;

    static const std::string ASSET_TYPE_INVALID;
    static const std::string ASSET_TYPE_NAMES[];

public:
    std::unique_ptr<AssetPool<IW4::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<IW4::PhysCollmap>> m_phys_collmap;
    std::unique_ptr<AssetPool<IW4::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<IW4::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<IW4::Material>> m_material;
    std::unique_ptr<AssetPool<IW4::MaterialPixelShader>> m_material_pixel_shader;
    std::unique_ptr<AssetPool<IW4::MaterialVertexShader>> m_material_vertex_shader;
    std::unique_ptr<AssetPool<IW4::MaterialVertexDeclaration>> m_material_vertex_decl;
    std::unique_ptr<AssetPool<IW4::MaterialTechniqueSet>> m_technique_set;
    std::unique_ptr<AssetPool<IW4::GfxImage>> m_image;
    // std::unique_ptr<AssetPool<IW4::snd_alias_list_t>> m_sound;
    // std::unique_ptr<AssetPool<IW4::SndCurve>> m_sound_curve;
    // std::unique_ptr<AssetPool<IW4::LoadedSound>> m_loaded_sound;
    // std::unique_ptr<AssetPool<IW4::clipMap_t>> m_clip_map;
    // std::unique_ptr<AssetPool<IW4::ComWorld>> m_com_world;
    // std::unique_ptr<AssetPool<IW4::GameWorldSp>> m_game_world_sp;
    // std::unique_ptr<AssetPool<IW4::GameWorldMp>> m_game_world_mp;
    // std::unique_ptr<AssetPool<IW4::MapEnts>> m_map_ents;
    // std::unique_ptr<AssetPool<IW4::FxWorld>> m_fx_world;
    // std::unique_ptr<AssetPool<IW4::GfxWorld>> m_gfx_world;
    // std::unique_ptr<AssetPool<IW4::GfxLightDef>> m_gfx_light_def;
    // std::unique_ptr<AssetPool<IW4::Font_s>> m_font;
    // std::unique_ptr<AssetPool<IW4::MenuList>> m_menu_list;
    // std::unique_ptr<AssetPool<IW4::menuDef_t>> m_menu_def;
    // std::unique_ptr<AssetPool<IW4::LocalizeEntry>> m_localize;
    // std::unique_ptr<AssetPool<IW4::WeaponCompleteDef>> m_weapon;
    // std::unique_ptr<AssetPool<IW4::SndDriverGlobals>> m_snd_driver_globals;
    // std::unique_ptr<AssetPool<IW4::FxEffectDef>> m_fx;
    // std::unique_ptr<AssetPool<IW4::FxImpactTable>> m_fx_impact_table;
    std::unique_ptr<AssetPool<IW4::RawFile>> m_raw_file;
    std::unique_ptr<AssetPool<IW4::StringTable>> m_string_table;
    // std::unique_ptr<AssetPool<IW4::LeaderboardDef>> m_leaderboard;
    // std::unique_ptr<AssetPool<IW4::StructuredDataDefSet>> m_structed_data_def_set;
    // std::unique_ptr<AssetPool<IW4::TracerDef>> m_tracer;
    // std::unique_ptr<AssetPool<IW4::VehicleDef>> m_vehicle;
    // std::unique_ptr<AssetPool<IW4::AddonMapEnts>> m_addon_map_ents;

    explicit GameAssetPoolIW4(int priority);
    ~GameAssetPoolIW4() override = default;

    void InitPoolStatic(asset_type_t type, size_t capacity) override;
    void InitPoolDynamic(asset_type_t type) override;

    XAssetInfoGeneric* AddAsset(asset_type_t type, std::string name, void* asset,
                                std::vector<std::string>& scriptStrings,
                                std::vector<XAssetInfoGeneric*>& dependencies) override;
    XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const override;
    const std::string& GetAssetTypeName(asset_type_t assetType) const override;

    iterator begin() const override;
    iterator end() const override;
};
