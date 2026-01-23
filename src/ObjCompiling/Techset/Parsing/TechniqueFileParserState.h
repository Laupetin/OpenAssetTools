#pragma once

#include "Techset/CommonTechnique.h"

#include <cstdint>
#include <memory>

namespace techset
{
    enum class TechniqueParserShaderType : std::uint8_t
    {
        VERTEX,
        PIXEL
    };

    class TechniqueParserState
    {
    public:
        TechniqueParserState(std::string techniqueName, const CommonCodeSourceInfos& codeSourceInfos, const CommonStreamRoutingInfos& routingInfos);

        std::unique_ptr<CommonTechnique> m_technique;

        const CommonCodeSourceInfos& m_code_source_infos;
        const CommonStreamRoutingInfos& m_routing_infos;

        std::optional<CommonPass> m_current_pass;
        std::optional<CommonTechniqueShader> m_current_shader;
        TechniqueParserShaderType m_current_shader_type;
    };
} // namespace techset
