#pragma once

#include "Dumping/IZoneAssetDumperState.h"

#include <unordered_set>

namespace techset
{
    class ShaderDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        bool ShouldDumpTechnique(const void* technique);
        bool ShouldDumpVertexShader(const void* vertexShader);
        bool ShouldDumpPixelShader(const void* pixelShader);

    private:
        std::unordered_set<const void*> m_dumped_techniques;
        std::unordered_set<const void*> m_dumped_vertex_shaders;
        std::unordered_set<const void*> m_dumped_pixel_shaders;
    };
} // namespace techset
