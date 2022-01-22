#include "StructuredDataDefParser.h"

#include "Sequence/StructuredDataDefScopeSequences.h"
#include "Sequence/StructuredDataEnumScopeSequences.h"
#include "Sequence/StructuredDataNoScopeSequences.h"
#include "Sequence/StructuredDataStructScopeSequences.h"

using namespace sdd;

StructuredDataDefParser::StructuredDataDefParser(SimpleLexer* lexer)
    : AbstractParser(lexer, std::make_unique<StructuredDataDefParserState>())
{
    CreateSequenceCollections();
}

void StructuredDataDefParser::CreateSequenceCollections()
{
    m_all_tests.clear();
    m_no_scope_tests.clear();
    m_def_tests.clear();
    m_enum_tests.clear();
    m_struct_tests.clear();

    const StructuredDataNoScopeSequences noScopeSequences(m_all_tests, m_no_scope_tests);
    noScopeSequences.AddSequences();

    const StructuredDataDefScopeSequences defScopeSequences(m_all_tests, m_def_tests);
    defScopeSequences.AddSequences();

    const StructuredDataEnumScopeSequences enumScopeSequences(m_all_tests, m_enum_tests);
    enumScopeSequences.AddSequences();

    const StructuredDataStructScopeSequences structScopeSequences(m_all_tests, m_struct_tests);
    structScopeSequences.AddSequences();
}

const std::vector<StructuredDataDefParser::sequence_t*>& StructuredDataDefParser::GetTestsForState()
{
    if (!m_state->m_current_def)
        return m_no_scope_tests;

    if (m_state->m_current_enum)
        return m_enum_tests;

    if (m_state->m_current_struct)
        return m_struct_tests;

    return m_def_tests;
}

std::vector<std::unique_ptr<CommonStructuredDataDef>> StructuredDataDefParser::GetDefs() const
{
    return std::move(m_state->m_defs);
}
