#include "MaterialCommon.h"

using namespace material;

CommonConstant::CommonConstant()
    : m_literal{}
{
}

CommonMaterial::CommonMaterial()
    : m_game_flags(0u),
      m_sort_key(0u),
      m_texture_atlas_row_count(0u),
      m_texture_atlas_column_count(0u),
      m_surface_flags(0u),
      m_contents(0u),
      m_ref_state_bits{}
{
}

CommonTexture::CommonTexture()
    : m_sampler_state(0u),
      m_semantic(0u)
{
}

CommonWaterDef::CommonWaterDef()
    : m_texture_width(0u),
      m_horizontal_world_length(0u),
      m_vertical_world_length(0u),
      m_amplitude(0u),
      m_wind_speed(0u),
      m_wind_direction{}
{
}
