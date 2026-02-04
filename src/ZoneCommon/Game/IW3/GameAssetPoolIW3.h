#pragma once

#include "Game/IW3/IW3.h"
#include "Pool/AssetPool.h"
#include "Pool/ZoneAssetPools.h"

#include <memory>

class GameAssetPoolIW3 final : public ZoneAssetPools
{
public:
    std::unique_ptr<AssetPool<IW3::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<IW3::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<IW3::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<IW3::Material>> m_material;
    std::unique_ptr<AssetPool<IW3::MaterialTechniqueSet>> m_technique_set;
    std::unique_ptr<AssetPool<IW3::GfxImage>> m_image;
    std::unique_ptr<AssetPool<IW3::snd_alias_list_t>> m_sound;
    std::unique_ptr<AssetPool<IW3::SndCurve>> m_sound_curve;
    std::unique_ptr<AssetPool<IW3::LoadedSound>> m_loaded_sound;
    std::unique_ptr<AssetPool<IW3::clipMap_t>> m_clip_map;
    std::unique_ptr<AssetPool<IW3::ComWorld>> m_com_world;
    std::unique_ptr<AssetPool<IW3::GameWorldSp>> m_game_world_sp;
    std::unique_ptr<AssetPool<IW3::GameWorldMp>> m_game_world_mp;
    std::unique_ptr<AssetPool<IW3::MapEnts>> m_map_ents;
    std::unique_ptr<AssetPool<IW3::GfxWorld>> m_gfx_world;
    std::unique_ptr<AssetPool<IW3::GfxLightDef>> m_gfx_light_def;
    std::unique_ptr<AssetPool<IW3::Font_s>> m_font;
    std::unique_ptr<AssetPool<IW3::MenuList>> m_menu_list;
    std::unique_ptr<AssetPool<IW3::menuDef_t>> m_menu_def;
    std::unique_ptr<AssetPool<IW3::LocalizeEntry>> m_localize;
    std::unique_ptr<AssetPool<IW3::WeaponDef>> m_weapon;
    std::unique_ptr<AssetPool<IW3::FxEffectDef>> m_fx;
    std::unique_ptr<AssetPool<IW3::FxImpactTable>> m_fx_impact_table;
    std::unique_ptr<AssetPool<IW3::RawFile>> m_raw_file;
    std::unique_ptr<AssetPool<IW3::StringTable>> m_string_table;

    GameAssetPoolIW3(Zone* zone, zone_priority_t priority);
    ~GameAssetPoolIW3() override = default;

    [[nodiscard]] XAssetInfoGeneric* GetAsset(asset_type_t type, const std::string& name) const override;

protected:
    XAssetInfoGeneric* AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;

private:
    zone_priority_t m_priority;
};
