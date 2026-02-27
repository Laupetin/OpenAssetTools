#include "TechniqueFileParser.h"

#include "Sequence/TechniqueNoScopeSequences.h"
#include "Sequence/TechniquePassScopeSequences.h"
#include "Sequence/TechniqueShaderScopeSequences.h"

namespace techset
{
    TechniqueParser::TechniqueParser(SimpleLexer& lexer,
                                     std::string techniqueName,
                                     const CommonCodeSourceInfos& codeSourceInfos,
                                     const CommonStreamRoutingInfos& routingInfos,
                                     CommonShaderArgCreator& shaderArgCreator)
        : AbstractParser(&lexer, std::make_unique<TechniqueParserState>(std::move(techniqueName), codeSourceInfos, routingInfos, shaderArgCreator))
    {
    }

    std::unique_ptr<CommonTechnique> TechniqueParser::GetParsingResult() const
    {
        return std::move(m_state->m_technique);
    }

    const std::vector<TechniqueParser::sequence_t*>& TechniqueParser::GetTestsForState()
    {
        if (m_state->m_current_shader.has_value())
            return TechniqueShaderScopeSequences::GetSequences();

        if (m_state->m_current_pass.has_value())
            return TechniquePassScopeSequences::GetSequences();

        return TechniqueNoScopeSequences::GetSequences();
    }
} // namespace techset
