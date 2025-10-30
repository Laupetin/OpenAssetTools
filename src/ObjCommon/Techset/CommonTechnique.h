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
        uint32_t m_version_major;
        uint32_t m_version_minor;
    };

    class CommonPass
    {
    public:
        uint64_t m_flags;
        uint32_t m_sampler_flags;
        CommonTechniqueShader m_vertex_shader;
        CommonTechniqueShader m_pixel_shader;
    };

    class CommonTechnique
    {
    public:
        uint64_t m_flags;
        std::vector<CommonPass> m_passes;
    };
} // namespace techset
