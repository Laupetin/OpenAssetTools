#include "TechniqueFileParserState.h"

namespace techset
{
    TechniqueParserState::TechniqueParserState(std::string techniqueName,
                                               const CommonCodeSourceInfos& codeSourceInfos,
                                               const CommonStreamRoutingInfos& routingInfos,
                                               CommonShaderArgCreator& shaderArgCreator)
        : m_technique(std::make_unique<CommonTechnique>(std::move(techniqueName))),
          m_code_source_infos(codeSourceInfos),
          m_routing_infos(routingInfos),
          m_shader_arg_creator(shaderArgCreator),
          m_current_shader_type(CommonTechniqueShaderType::VERTEX)
    {
    }
} // namespace techset
