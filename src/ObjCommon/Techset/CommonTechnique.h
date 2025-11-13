#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace techset
{
    class CommonTechniqueShader
    {
    public:
        std::string m_name;
        const void* m_shader_bin;
        size_t m_shader_bin_size;
    };

    enum class DxVersion : std::uint8_t
    {
        DX9,
        DX11
    };

    class CommonPass
    {
    public:
        uint32_t m_sampler_flags;
        DxVersion m_dx_version;
        CommonTechniqueShader m_vertex_shader;
        CommonTechniqueShader m_pixel_shader;
    };

    class CommonTechnique
    {
    public:
        std::string m_name;
        uint64_t m_flags;
        std::vector<CommonPass> m_passes;
    };
} // namespace techset
