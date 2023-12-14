#include "FunctionScopeSequences.h"

#include "Generic/GenericExpressionPropertySequence.h"
#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <sstream>

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
                create.Char('}').Capture(CAPTURE_TOKEN),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            if (!state->m_current_function->m_value)
            {
                std::ostringstream ss;
                ss << "Cannot define function name \"" << state->m_current_function->m_name << "\" without a value";
                throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
            }

            auto lowerCaseName = state->m_current_function->m_name;
            utils::MakeStringLowerCase(lowerCaseName);

            const auto existingFunction = state->m_functions_by_name.find(lowerCaseName);
            if (existingFunction == state->m_functions_by_name.end())
            {
                state->m_functions_by_name.emplace(std::make_pair(lowerCaseName, state->m_current_function));
                state->m_current_function = nullptr;
            }
            else if (!state->m_current_function->m_value->Equals(existingFunction->second->m_value.get()))
            {
                std::ostringstream ss;
                ss << "Function with name \"" << state->m_current_function->m_name << "\" already exists";
                throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
            }
            else
            {
                // Remove definition of function to not re-add it
                const auto foundFunction = std::find_if(state->m_functions.rbegin(),
                                                        state->m_functions.rend(),
                                                        [state](const auto& element)
                                                        {
                                                            return element.get() == state->m_current_function;
                                                        });

                assert(foundFunction != state->m_functions.rend());
                assert((foundFunction + 1).base()->get() == state->m_current_function);
                if (foundFunction != state->m_functions.rend())
                    state->m_functions.erase((foundFunction + 1).base());

                state->m_current_function = nullptr;
            }
        }
    };
} // namespace menu::function_scope_sequences

using namespace function_scope_sequences;

FunctionScopeSequences::FunctionScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences,
                                               std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void FunctionScopeSequences::AddSequences(FeatureLevel featureLevel, bool permissive) const
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("name",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_function->m_name = value;
                                                                }));
    AddSequence(GenericExpressionPropertySequence::WithKeyword("value",
                                                               [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
                                                               {
                                                                   state->m_current_function->m_value = std::move(value);
                                                               }));
}
