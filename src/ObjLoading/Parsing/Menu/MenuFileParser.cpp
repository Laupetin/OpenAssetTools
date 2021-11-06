#include "MenuFileParser.h"

#include "Sequence/EventHandlerSetScopeSequences.h"
#include "Sequence/FunctionScopeSequences.h"
#include "Sequence/GlobalScopeSequences.h"
#include "Sequence/ItemScopeSequences.h"
#include "Sequence/MenuScopeSequences.h"
#include "Sequence/NoScopeSequences.h"

using namespace menu;

MenuFileParser::MenuFileParser(SimpleLexer* lexer, const FeatureLevel featureLevel)
    : AbstractParser(lexer, std::make_unique<MenuFileParserState>(featureLevel))
{
    CreateSequenceCollections();
}

void MenuFileParser::AddSequence(std::vector<sequence_t*>& collection, std::unique_ptr<sequence_t> test)
{
    collection.push_back(test.get());
    m_all_tests.emplace_back(std::move(test));
}

void MenuFileParser::CreateSequenceCollections()
{
    m_all_tests.clear();
    m_no_scope_tests.clear();
    m_global_scope_tests.clear();
    m_function_scope_tests.clear();
    m_menu_scope_tests.clear();
    m_item_scope_tests.clear();
    m_event_handler_set_scope_tests.clear();

    const auto featureLevel = m_state->m_feature_level;

    NoScopeSequences noScopeSequences(m_all_tests, m_no_scope_tests);
    noScopeSequences.AddSequences(featureLevel);

    GlobalScopeSequences globalScopeSequences(m_all_tests, m_global_scope_tests);
    globalScopeSequences.AddSequences(featureLevel);

    MenuScopeSequences menuPropertySequences(m_all_tests, m_menu_scope_tests);
    menuPropertySequences.AddSequences(featureLevel);

    ItemScopeSequences itemPropertySequences(m_all_tests, m_item_scope_tests);
    itemPropertySequences.AddSequences(featureLevel);

    FunctionScopeSequences functionPropertySequences(m_all_tests, m_function_scope_tests);
    functionPropertySequences.AddSequences(featureLevel);

    EventHandlerSetScopeSequences eventHandlerSetScopeSequences(m_all_tests, m_event_handler_set_scope_tests);
    eventHandlerSetScopeSequences.AddSequences(featureLevel);
}

const std::vector<MenuFileParser::sequence_t*>& MenuFileParser::GetTestsForState()
{
    if (m_state->m_current_event_handler_set)
        return m_event_handler_set_scope_tests;

    if (m_state->m_current_item)
        return m_item_scope_tests;

    if (m_state->m_current_function)
        return m_function_scope_tests;

    if (m_state->m_current_menu)
        return m_menu_scope_tests;

    if (m_state->m_in_global_scope)
        return m_global_scope_tests;

    return m_no_scope_tests;
}

MenuFileParserState* MenuFileParser::GetState() const
{
    return m_state.get();
}
