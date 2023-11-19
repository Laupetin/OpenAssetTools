#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"
#include "StructuredDataDefParserState.h"
#include "Utils/ClassUtils.h"

namespace sdd
{
    class StructuredDataDefParser final : public AbstractParser<SimpleParserValue, StructuredDataDefParserState>
    {
        std::vector<std::unique_ptr<sequence_t>> m_all_tests;
        std::vector<sequence_t*> m_no_scope_tests;
        std::vector<sequence_t*> m_def_tests;
        std::vector<sequence_t*> m_enum_tests;
        std::vector<sequence_t*> m_struct_tests;

        void CreateSequenceCollections();

    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        explicit StructuredDataDefParser(SimpleLexer* lexer);
        _NODISCARD std::vector<std::unique_ptr<CommonStructuredDataDef>> GetDefs() const;
    };
} // namespace sdd
