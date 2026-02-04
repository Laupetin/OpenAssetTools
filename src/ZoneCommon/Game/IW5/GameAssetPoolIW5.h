#pragma once

#include "Game/IW5/IW5.h"
#include "Pool/AssetPool.h"
#include "Pool/ZoneAssetPools.h"

#include <memory>

class GameAssetPoolIW5 final : public ZoneAssetPools
{
public:
    std::unique_ptr<AssetPool<IW5::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<IW5::PhysCollmap>> m_phys_collmap;
    std::unique_ptr<AssetPool<IW5::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<IW5::XModelSurfs>> m_xmodel_surfs;
    std::unique_ptr<AssetPool<IW5::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<IW5::Material>> m_material;
    std::unique_ptr<AssetPool<IW5::MaterialPixelShader>> m_material_pixel_shader;
    std::unique_ptr<AssetPool<IW5::MaterialVertexShader>> m_material_vertex_shader;
    std::unique_ptr<AssetPool<IW5::MaterialVertexDeclaration>> m_material_vertex_decl;
    std::unique_ptr<AssetPool<IW5::MaterialTechniqueSet>> m_technique_set;
    std::unique_ptr<AssetPool<IW5::GfxImage>> m_image;
    std::unique_ptr<AssetPool<IW5::snd_alias_list_t>> m_sound;
    std::unique_ptr<AssetPool<IW5::SndCurve>> m_sound_curve;
    std::unique_ptr<AssetPool<IW5::LoadedSound>> m_loaded_sound;
    std::unique_ptr<AssetPool<IW5::clipMap_t>> m_clip_map;
    std::unique_ptr<AssetPool<IW5::ComWorld>> m_com_world;
    std::unique_ptr<AssetPool<IW5::GlassWorld>> m_glass_world;
    std::unique_ptr<AssetPool<IW5::PathData>> m_path_data;
    std::unique_ptr<AssetPool<IW5::VehicleTrack>> m_vehicle_track;
    std::unique_ptr<AssetPool<IW5::MapEnts>> m_map_ents;
    std::unique_ptr<AssetPool<IW5::FxWorld>> m_fx_world;
    std::unique_ptr<AssetPool<IW5::GfxWorld>> m_gfx_world;
    std::unique_ptr<AssetPool<IW5::GfxLightDef>> m_gfx_light_def;
    std::unique_ptr<AssetPool<IW5::Font_s>> m_font;
    std::unique_ptr<AssetPool<IW5::MenuList>> m_menu_list;
    std::unique_ptr<AssetPool<IW5::menuDef_t>> m_menu_def;
    std::unique_ptr<AssetPool<IW5::LocalizeEntry>> m_localize;
    std::unique_ptr<AssetPool<IW5::WeaponAttachment>> m_attachment;
    std::unique_ptr<AssetPool<IW5::WeaponCompleteDef>> m_weapon;
    std::unique_ptr<AssetPool<IW5::FxEffectDef>> m_fx;
    std::unique_ptr<AssetPool<IW5::FxImpactTable>> m_fx_impact_table;
    std::unique_ptr<AssetPool<IW5::SurfaceFxTable>> m_surface_fx_table;
    std::unique_ptr<AssetPool<IW5::RawFile>> m_raw_file;
    std::unique_ptr<AssetPool<IW5::ScriptFile>> m_script_file;
    std::unique_ptr<AssetPool<IW5::StringTable>> m_string_table;
    std::unique_ptr<AssetPool<IW5::LeaderboardDef>> m_leaderboard;
    std::unique_ptr<AssetPool<IW5::StructuredDataDefSet>> m_structed_data_def_set;
    std::unique_ptr<AssetPool<IW5::TracerDef>> m_tracer;
    std::unique_ptr<AssetPool<IW5::VehicleDef>> m_vehicle;
    std::unique_ptr<AssetPool<IW5::AddonMapEnts>> m_addon_map_ents;

    GameAssetPoolIW5(Zone* zone, zone_priority_t priority);
    ~GameAssetPoolIW5() override = default;

    [[nodiscard]] XAssetInfoGeneric* GetAsset(asset_type_t type, const std::string& name) const override;

protected:
    XAssetInfoGeneric* AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;

private:
    zone_priority_t m_priority;
};
