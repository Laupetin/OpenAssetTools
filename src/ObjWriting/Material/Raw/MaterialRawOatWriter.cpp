#include "MaterialRawOatWriter.h"

#include "Material/Raw/MaterialRawOat.h"

using namespace material;

MaterialRawOatWriter::MaterialRawOatWriter(std::ostream& stream)
    : AbstractRawDumper(stream)
{
}

void MaterialRawOatWriter::WriteMaterial(const CommonMaterial& material)
{
    constexpr MaterialRawOatHeader header{
        {'M', 'T', 'L'},
        1u
    };
    OffsetAdd(sizeof(header));

    MaterialRaw materialRaw{};
    OffsetAdd(sizeof(materialRaw));

    materialRaw.info.nameOffset = OffsetAdd(material.m_name);
    materialRaw.info.gameFlags = material.m_game_flags;
    materialRaw.info.sortKey = material.m_sort_key;
    materialRaw.info.textureAtlasRowCount = material.m_texture_atlas_row_count;
    materialRaw.info.textureAtlasColumnCount = material.m_texture_atlas_column_count;
    materialRaw.info.surfaceFlags = material.m_surface_flags;
    materialRaw.info.contents = material.m_contents;
    materialRaw.refStateBits[0] = material.m_ref_state_bits[0];
    materialRaw.refStateBits[1] = material.m_ref_state_bits[1];
    materialRaw.textureCount = static_cast<uint16_t>(material.m_textures.size());
    materialRaw.constantCount = static_cast<uint16_t>(material.m_textures.size());
    materialRaw.techSetNameOffset = OffsetAdd(material.m_techset_name);
    materialRaw.textureTableOffset = OffsetAdd(sizeof(MaterialTextureDefRaw) * material.m_textures.size());
    materialRaw.constantTableOffset = OffsetAdd(sizeof(MaterialConstantDefRaw) * material.m_textures.size());

    const auto textureCount = material.m_textures.size();
    std::vector<MaterialTextureDefRaw> textureDefs(textureCount);
    for (auto i = 0u; i < textureCount; i++)
    {
        const auto& texture = material.m_textures[i];
        auto& textureDef = textureDefs[i];

        textureDef.nameOffset = OffsetAdd(texture.m_name);
        textureDef.samplerState = texture.m_sampler_state;
        textureDef.semantic = texture.m_semantic;
        textureDef.imageNameOffset = OffsetAdd(texture.m_image_name);
        textureDef.waterDefOffset = texture.m_water_def ? OffsetAdd(sizeof(MaterialWaterDefRaw)) : 0u;
    }

    const auto constantCount = material.m_constants.size();
    std::vector<MaterialConstantDefRaw> constantDefs(constantCount);
    for (auto i = 0u; i < constantCount; i++)
    {
        const auto& constant = material.m_constants[i];
        auto& constantDef = constantDefs[i];

        constantDef.nameOffset = OffsetAdd(constant.m_name);
        constantDef.literal[0] = constant.m_literal[0];
        constantDef.literal[1] = constant.m_literal[1];
        constantDef.literal[2] = constant.m_literal[2];
        constantDef.literal[3] = constant.m_literal[3];
    }

    Write(&header, sizeof(header));
    Write(&materialRaw, sizeof(materialRaw));
    Write(material.m_name);
    Write(material.m_techset_name);
    Write(textureDefs.data(), sizeof(MaterialTextureDefRaw) * textureCount);
    Write(constantDefs.data(), sizeof(MaterialConstantDefRaw) * constantCount);

    for (auto i = 0u; i < textureCount; i++)
    {
        const auto& texture = material.m_textures[i];

        Write(texture.m_name);
        Write(texture.m_image_name);

        if (texture.m_water_def)
        {
            const auto& water = *texture.m_water_def;
            MaterialWaterDefRaw waterDef{};
            waterDef.textureWidth = water.m_texture_width;
            waterDef.horizontalWorldLength = water.m_horizontal_world_length;
            waterDef.verticalWorldLength = water.m_vertical_world_length;
            waterDef.amplitude = water.m_amplitude;
            waterDef.windSpeed = water.m_wind_speed;
            waterDef.windDirection[0] = water.m_wind_direction[0];
            waterDef.windDirection[1] = water.m_wind_direction[1];

            Write(&waterDef, sizeof(MaterialWaterDefRaw));
        }
    }

    for (auto i = 0u; i < constantCount; i++)
    {
        const auto& constant = material.m_constants[i];
        Write(constant.m_name);
    }
}
