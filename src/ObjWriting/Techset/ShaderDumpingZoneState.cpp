#include "ShaderDumpingZoneState.h"

namespace techset
{
    bool ShaderDumpingZoneState::ShouldDumpTechnique(const void* technique)
    {
        if (m_dumped_techniques.contains(technique))
            return false;

        m_dumped_techniques.emplace(technique);
        return true;
    }

    bool ShaderDumpingZoneState::ShouldDumpVertexShader(const void* vertexShader)
    {
        if (m_dumped_vertex_shaders.contains(vertexShader))
            return false;

        m_dumped_vertex_shaders.emplace(vertexShader);
        return true;
    }

    bool ShaderDumpingZoneState::ShouldDumpPixelShader(const void* pixelShader)
    {
        if (m_dumped_pixel_shaders.contains(pixelShader))
            return false;

        m_dumped_pixel_shaders.emplace(pixelShader);
        return true;
    }
} // namespace techset
