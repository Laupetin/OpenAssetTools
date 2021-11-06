#include "FunctionScopeSequences.h"

#include <sstream>

#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::function_scope_sequences
{
    class SequenceCloseBlock final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_TOKEN = 1;

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
            const auto existingFunction = state->m_functions_by_name.find(state->m_current_function->m_name);
            if (existingFunction == state->m_functions_by_name.end())
            {
                state->m_functions_by_name.emplace(std::make_pair(state->m_current_function->m_name, state->m_current_function.get()));
                state->m_functions.emplace_back(std::move(state->m_current_function));
                state->m_current_function = nullptr;
            }
            else
            {
                std::ostringstream ss;
                ss << "Function with name \"" << state->m_current_menu->m_name << "\" already exists";
                throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
            }
        }
    };
}

using namespace function_scope_sequences;

FunctionScopeSequences::FunctionScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void FunctionScopeSequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_function->m_name = value;
    }));
}
