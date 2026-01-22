#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "TechsetFileParserState.h"

namespace techset
{
    class TechsetParser final : public AbstractParser<SimpleParserValue, TechsetParserState>
    {
    public:
        TechsetParser(SimpleLexer& lexer, std::string techsetName, const CommonTechniqueTypeNames& techniqueTypeNames);
        [[nodiscard]] std::unique_ptr<CommonTechset> GetParsingResult() const;

    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;
    };
} // namespace techset
