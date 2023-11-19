#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Techset/TechsetDefinition.h"
#include "TechsetFileParserState.h"
#include "Utils/ClassUtils.h"

namespace techset
{
    class TechsetParser final : public AbstractParser<SimpleParserValue, TechsetParserState>
    {
    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        TechsetParser(SimpleLexer* lexer, const char** validTechniqueTypeNames, size_t validTechniqueTypeNameCount);
        _NODISCARD std::unique_ptr<TechsetDefinition> GetTechsetDefinition() const;
    };
} // namespace techset
