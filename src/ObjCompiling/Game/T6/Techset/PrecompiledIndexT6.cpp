#include "PrecompiledIndexT6.h"

#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Techset/CommonTechnique.h"

#include <algorithm>
#include <vector>

using namespace T6;

namespace
{
    class PrecompiledIndexMapping
    {
    public:
        VertexShaderPrecompiledIndex m_value;
        std::optional<MaterialType> m_material_type;
        std::vector<MaterialConstantSource> m_vertex_const_constraints;
        std::vector<CustomSampler> m_custom_sampler_constraints;
    };

    // clang-format off
    inline PrecompiledIndexMapping precompiledIndexMappings[]{
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },                                       
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
            },
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
            },
            .m_custom_sampler_constraints = {},
         },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            }, 
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
            },                                              
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            },
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT,
            .m_material_type = MTL_TYPE_MODEL_VERTCOL,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            },
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
            },                 
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
            }, 
            .m_custom_sampler_constraints = {},
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            },
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            }, 
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
            },
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            },
        },
        {
            .m_value = VERTEX_SHADER_MODEL_LIT_LIGHTMAP_VC,
            .m_material_type = MTL_TYPE_MODEL_LIGHTMAP_VC,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
                CONST_SRC_CODE_GRID_LIGHTING_SH_0,
                CONST_SRC_CODE_GRID_LIGHTING_SH_1,
                CONST_SRC_CODE_GRID_LIGHTING_SH_2,
            }, 
            .m_custom_sampler_constraints = {
                CUSTOM_SAMPLER_REFLECTION_PROBE,
            }, 
        },
        {
            .m_value = VERTEX_SHADER_MODEL_UNLIT,
            .m_material_type = std::nullopt,
            .m_vertex_const_constraints = {
                CONST_SRC_CODE_WORLD_MATRIX,
            },                                              
            .m_custom_sampler_constraints = {},
        },
    };
    // clang-format on

    [[nodiscard]] bool MappingMatches(const PrecompiledIndexMapping& mapping,
                                      const MaterialType materialType,
                                      const std::vector<techset::CommonStreamSource>& passCodeConsts,
                                      const unsigned samplerFlags)
    {
        if (mapping.m_material_type && *mapping.m_material_type != materialType)
            return false;
        if (mapping.m_vertex_const_constraints.size() > passCodeConsts.size())
            return false;

        auto passCodeConstIndex = 0u;
        for (const auto codeConstConstraint : mapping.m_vertex_const_constraints)
        {
            if (passCodeConsts[passCodeConstIndex++] != codeConstConstraint)
                return false;
        }

        auto expectedSamplerFlags = 0u;
        for (const auto customSampler : mapping.m_custom_sampler_constraints)
            expectedSamplerFlags |= (1 << customSampler);

        return samplerFlags == expectedSamplerFlags;
    }
} // namespace

namespace T6
{
    void ApplyPrecompiledIndex(MaterialPass& pass)
    {
        if (!pass.args)
            return;

        std::vector<techset::CommonStreamSource> codeConstants;
        const auto argCount = static_cast<unsigned>(pass.perPrimArgCount);
        for (auto argIndex = 0u; argIndex < argCount; argIndex++)
        {
            const auto& arg = pass.args[argIndex];
            if (arg.type != MTL_ARG_CODE_VERTEX_CONST)
                continue;

            const auto codeConst = static_cast<techset::CommonStreamSource>(arg.u.codeConst.index);

            // Make sure we are agnostic of the used transposed versions
            const auto codeConstInfo = commonCodeSourceInfos.GetInfoForCodeConstSource(codeConst);
            if (codeConstInfo && codeConstInfo->transposedMatrix)
                codeConstants.emplace_back(std::min(codeConst, *codeConstInfo->transposedMatrix));
            else
                codeConstants.emplace_back(codeConst);
        }

        const auto end = std::end(precompiledIndexMappings);
        const auto foundMapping = std::find_if(std::begin(precompiledIndexMappings),
                                               end,
                                               [&codeConstants, pass](const PrecompiledIndexMapping& mapping) -> bool
                                               {
                                                   return MappingMatches(mapping, pass.materialType, codeConstants, pass.customSamplerFlags);
                                               });

        if (foundMapping != end)
            pass.precompiledIndex = foundMapping->m_value;
    }
} // namespace T6
