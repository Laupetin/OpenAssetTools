#include "MenuFileParser.h"

#include "Sequence/SequenceCloseBlock.h"
#include "Sequence/SequenceFunctionDef.h"
#include "Sequence/SequenceItemDef.h"
#include "Sequence/SequenceLoadMenu.h"
#include "Sequence/SequenceMenuDef.h"
#include "Sequence/SequenceOpenGlobalScopeBlock.h"
#include "Sequence/Properties/FunctionPropertySequences.h"
#include "Sequence/Properties/ItemPropertySequences.h"
#include "Sequence/Properties/MenuPropertySequences.h"

using namespace menu;

MenuFileParser::MenuFileParser(SimpleLexer* lexer, const FeatureLevel featureLevel)
    : AbstractParser(lexer, std::make_unique<MenuFileParserState>(featureLevel))
{
    CreateTestCollections();
}

void MenuFileParser::AddSequence(std::vector<sequence_t*>& collection, std::unique_ptr<sequence_t> test)
{
    collection.push_back(test.get());
    m_all_tests.emplace_back(std::move(test));
}

void MenuFileParser::CreateNoScopeTests()
{
    AddSequence(m_no_scope_tests, std::make_unique<SequenceOpenGlobalScopeBlock>());
}

void MenuFileParser::CreateGlobalScopeTests()
{
    AddSequence(m_global_scope_tests, std::make_unique<SequenceCloseBlock>());
    AddSequence(m_global_scope_tests, std::make_unique<SequenceMenuDef>());
    AddSequence(m_global_scope_tests, std::make_unique<SequenceFunctionDef>());
    AddSequence(m_global_scope_tests, std::make_unique<SequenceLoadMenu>());
}

void MenuFileParser::CreateFunctionScopeTests()
{
    AddSequence(m_function_scope_tests, std::make_unique<SequenceCloseBlock>());

    FunctionPropertySequences functionPropertySequences(m_all_tests, m_function_scope_tests);
    functionPropertySequences.AddSequences(m_state->m_feature_level);
}

void MenuFileParser::CreateMenuScopeTests()
{
    AddSequence(m_menu_scope_tests, std::make_unique<SequenceCloseBlock>());
    AddSequence(m_menu_scope_tests, std::make_unique<SequenceItemDef>());

    MenuPropertySequences menuPropertySequences(m_all_tests, m_menu_scope_tests);
    menuPropertySequences.AddSequences(m_state->m_feature_level);
}

void MenuFileParser::CreateItemScopeTests()
{
    AddSequence(m_item_scope_tests, std::make_unique<SequenceCloseBlock>());

    ItemPropertySequences itemPropertySequences(m_all_tests, m_item_scope_tests);
    itemPropertySequences.AddSequences(m_state->m_feature_level);
}

void MenuFileParser::CreateTestCollections()
{
    m_all_tests.clear();
    m_no_scope_tests.clear();
    m_global_scope_tests.clear();
    m_function_scope_tests.clear();
    m_menu_scope_tests.clear();
    m_item_scope_tests.clear();

    CreateNoScopeTests();
    CreateGlobalScopeTests();
    CreateFunctionScopeTests();
    CreateMenuScopeTests();
    CreateItemScopeTests();
}

const std::vector<MenuFileParser::sequence_t*>& MenuFileParser::GetTestsForState()
{
    if (!m_state->m_in_global_scope)
        return m_no_scope_tests;

    if (m_state->m_current_item)
        return m_item_scope_tests;

    if (m_state->m_current_function)
        return m_function_scope_tests;

    if (m_state->m_current_menu)
        return m_menu_scope_tests;

    return m_global_scope_tests;
}

MenuFileParserState* MenuFileParser::GetState() const
{
    return m_state.get();
}
