#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "TechniqueFileParserState.h"
#include "Techset/CommonTechnique.h"

namespace techset
{
    class TechniqueParser final : public AbstractParser<SimpleParserValue, TechniqueParserState>
    {
    public:
        TechniqueParser(SimpleLexer& lexer,
                        std::string techniqueName,
                        const CommonCodeSourceInfos& codeSourceInfos,
                        const CommonStreamRoutingInfos& routingInfos,
                        CommonShaderArgCreator& shaderArgCreator);
        [[nodiscard]] std::unique_ptr<CommonTechnique> GetParsingResult() const;

    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;
    };
} // namespace techset
