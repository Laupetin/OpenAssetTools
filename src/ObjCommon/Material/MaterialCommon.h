#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace material
{
    class CommonWaterDef
    {
    public:
        int m_texture_width;
        float m_horizontal_world_length;
        float m_vertical_world_length;
        float m_amplitude;
        float m_wind_speed;
        float m_wind_direction[2];

        CommonWaterDef();
    };

    class CommonTexture
    {
    public:
        std::string m_name;
        uint8_t m_sampler_state;
        uint8_t m_semantic;
        std::string m_image_name;
        std::unique_ptr<CommonWaterDef> m_water_def;

        CommonTexture();
    };

    class CommonConstant
    {
    public:
        std::string m_name;
        float m_literal[4];

        CommonConstant();
    };

    class CommonMaterial
    {
    public:
        std::string m_name;
        uint8_t m_game_flags;
        uint8_t m_sort_key;
        uint8_t m_texture_atlas_row_count;
        uint8_t m_texture_atlas_column_count;
        uint32_t m_surface_flags;
        uint32_t m_contents;
        uint32_t m_ref_state_bits[2];
        std::vector<CommonTexture> m_textures;
        std::vector<CommonConstant> m_constants;
        std::string m_techset_name;

        CommonMaterial();
    };
}
