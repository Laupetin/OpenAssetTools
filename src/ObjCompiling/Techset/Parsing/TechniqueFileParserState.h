#pragma once

#include "Techset/CommonShaderArgCreator.h"
#include "Techset/CommonTechnique.h"

#include <memory>

namespace techset
{
    class TechniqueParserState
    {
    public:
        TechniqueParserState(std::string techniqueName,
                             const CommonCodeSourceInfos& codeSourceInfos,
                             const CommonStreamRoutingInfos& routingInfos,
                             CommonShaderArgCreator& shaderArgCreator);

        std::unique_ptr<CommonTechnique> m_technique;

        const CommonCodeSourceInfos& m_code_source_infos;
        const CommonStreamRoutingInfos& m_routing_infos;
        CommonShaderArgCreator& m_shader_arg_creator;

        std::optional<CommonPass> m_current_pass;
        std::optional<CommonTechniqueShader> m_current_shader;
        CommonTechniqueShaderType m_current_shader_type;
    };
} // namespace techset
