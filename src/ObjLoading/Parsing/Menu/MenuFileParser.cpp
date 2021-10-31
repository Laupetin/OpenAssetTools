#include "MenuFileParser.h"

#include "Sequence/SequenceCloseBlock.h"
#include "Sequence/SequenceFunctionDef.h"
#include "Sequence/SequenceItemDef.h"
#include "Sequence/SequenceLoadMenu.h"
#include "Sequence/SequenceMenuDef.h"
#include "Sequence/SequenceOpenGlobalScopeBlock.h"
#include "Sequence/Properties/SequenceName.h"

MenuFileParser::MenuFileParser(SimpleLexer* lexer, const MenuFeatureLevel featureLevel)
    : AbstractParser(lexer, std::make_unique<MenuFileParserState>(featureLevel))
{
    CreateTestCollections();
}

void MenuFileParser::AddTest(std::vector<sequence_t*>& collection, std::unique_ptr<sequence_t> test)
{
    collection.push_back(test.get());
    m_all_tests.emplace_back(std::move(test));
}

void MenuFileParser::CreateNoScopeTests()
{
    AddTest(m_no_scope_tests, std::make_unique<SequenceOpenGlobalScopeBlock>());
}

void MenuFileParser::CreateGlobalScopeTests()
{
    AddTest(m_global_scope_tests, std::make_unique<SequenceCloseBlock>());
    AddTest(m_global_scope_tests, std::make_unique<SequenceMenuDef>());
    AddTest(m_global_scope_tests, std::make_unique<SequenceFunctionDef>());
    AddTest(m_global_scope_tests, std::make_unique<SequenceLoadMenu>());
}

void MenuFileParser::CreateFunctionScopeTests()
{
    AddTest(m_function_scope_tests, std::make_unique<SequenceName>());
    AddTest(m_function_scope_tests, std::make_unique<SequenceCloseBlock>());
}

void MenuFileParser::CreateMenuScopeTests()
{
    AddTest(m_menu_scope_tests, std::make_unique<SequenceName>());
    AddTest(m_menu_scope_tests, std::make_unique<SequenceCloseBlock>());
    AddTest(m_menu_scope_tests, std::make_unique<SequenceItemDef>());
}

void MenuFileParser::CreateItemScopeTests()
{
    AddTest(m_item_scope_tests, std::make_unique<SequenceName>());
    AddTest(m_item_scope_tests, std::make_unique<SequenceCloseBlock>());
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
