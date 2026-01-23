#include "TechniqueFileParserState.h"

namespace techset
{
    TechniqueParserState::TechniqueParserState(std::string techniqueName,
                                               const CommonCodeSourceInfos& codeSourceInfos,
                                               const CommonStreamRoutingInfos& routingInfos)
        : m_technique(std::make_unique<CommonTechnique>(std::move(techniqueName))),
          m_code_source_infos(codeSourceInfos),
          m_routing_infos(routingInfos),
          m_current_shader_type(TechniqueParserShaderType::VERTEX)
    {
    }
} // namespace techset
