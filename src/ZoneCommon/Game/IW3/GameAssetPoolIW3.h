#pragma once

#include <memory>

#include "Pool/ZoneAssetPools.h"
#include "Pool/AssetPool.h"
#include "Game/IW3/IW3.h"

class GameAssetPoolIW3 final : public ZoneAssetPools
{
    int m_priority;

    static constexpr const char* ASSET_TYPE_INVALID = "invalid_asset_type";
    static const char* ASSET_TYPE_NAMES[];

protected:
    XAssetInfoGeneric* AddAssetToPool(asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings, Zone* zone) override;

public:
    std::unique_ptr<AssetPool<IW3::PhysPreset>> m_phys_preset;
    std::unique_ptr<AssetPool<IW3::XAnimParts>> m_xanim_parts;
    std::unique_ptr<AssetPool<IW3::XModel>> m_xmodel;
    std::unique_ptr<AssetPool<IW3::Material>> m_material;
    std::unique_ptr<AssetPool<IW3::MaterialPixelShader>> m_material_pixel_shader;
    std::unique_ptr<AssetPool<IW3::MaterialVertexShader>> m_material_vertex_shader;
    std::unique_ptr<AssetPool<IW3::MaterialVertexDeclaration>> m_material_vertex_decl;
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

    GameAssetPoolIW3(Zone* zone, int priority);
    ~GameAssetPoolIW3() override = default;

    void InitPoolStatic(asset_type_t type, size_t capacity) override;
    void InitPoolDynamic(asset_type_t type) override;

    _NODISCARD XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const override;
    static const char* AssetTypeNameByType(asset_type_t assetType);
    _NODISCARD const char* GetAssetTypeName(asset_type_t assetType) const override;
};
