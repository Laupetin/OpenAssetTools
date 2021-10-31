#pragma once

#include "Utils/ClassUtils.h"
#include "MenuFileParserState.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Impl/AbstractParser.h"

class MenuFileParser final : public AbstractParser<SimpleParserValue, MenuFileParserState>
{
    std::vector<std::unique_ptr<sequence_t>> m_all_tests;
    std::vector<sequence_t*> m_no_scope_tests;
    std::vector<sequence_t*> m_global_scope_tests;
    std::vector<sequence_t*> m_function_scope_tests;
    std::vector<sequence_t*> m_menu_scope_tests;
    std::vector<sequence_t*> m_item_scope_tests;

    void AddTest(std::vector<sequence_t*>& collection, std::unique_ptr<sequence_t> test);
    void CreateNoScopeTests();
    void CreateGlobalScopeTests();
    void CreateFunctionScopeTests();
    void CreateMenuScopeTests();
    void CreateItemScopeTests();
    void CreateTestCollections();

protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    MenuFileParser(SimpleLexer* lexer, MenuFeatureLevel featureLevel);
    _NODISCARD MenuFileParserState* GetState() const;
};
