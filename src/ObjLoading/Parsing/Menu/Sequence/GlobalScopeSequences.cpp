#include "GlobalScopeSequences.h"

#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::global_scope_sequences
{
    class SequenceCloseBlock final : public MenuFileParser::sequence_t
    {
    public:
        SequenceCloseBlock()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char('}')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_in_global_scope = false;
        }
    };

    class SequenceFunctionDef final : public MenuFileParser::sequence_t
    {
    public:
        SequenceFunctionDef()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Keyword("functionDef"),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            auto newFunction = std::make_unique<CommonFunctionDef>();
            state->m_current_function = newFunction.get();
            state->m_functions.emplace_back(std::move(newFunction));
        }
    };

    class SequenceMenuDef final : public MenuFileParser::sequence_t
    {
    public:
        SequenceMenuDef()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Keyword("menuDef"),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            auto newMenu = std::make_unique<CommonMenuDef>();
            state->m_current_menu = newMenu.get();
            state->m_menus.emplace_back(std::move(newMenu));
        }
    };

    class SequenceLoadMenu final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_MENU_NAME = 1;

    public:
        SequenceLoadMenu()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Keyword("loadMenu"),
                create.Char('{'),
                create.StringChain().Capture(CAPTURE_MENU_NAME),
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& menuNameToken = result.NextCapture(CAPTURE_MENU_NAME);

            if (menuNameToken.StringValue().empty())
                throw ParsingException(menuNameToken.GetPos(), "Invalid menu name");

            state->m_menus_to_load.emplace_back(menuNameToken.StringValue());
        }
    };
}

using namespace global_scope_sequences;

GlobalScopeSequences::GlobalScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void GlobalScopeSequences::AddSequences(FeatureLevel featureLevel, bool permissive)
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<SequenceFunctionDef>());
    AddSequence(std::make_unique<SequenceMenuDef>());
    AddSequence(std::make_unique<SequenceLoadMenu>());
}
