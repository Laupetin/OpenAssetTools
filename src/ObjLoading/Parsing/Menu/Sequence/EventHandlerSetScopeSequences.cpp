#include "EventHandlerSetScopeSequences.h"

#include <sstream>

#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerScript.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::event_handler_set_scope_sequences
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
            auto remainingScript = state->m_current_script.str();
            if (!remainingScript.empty())
                state->m_current_nested_event_handler_set->m_elements.emplace_back(std::make_unique<CommonEventHandlerScript>(std::move(remainingScript)));
            state->m_current_script.clear();

            if (!state->m_condition_stack.empty())
            {
                state->m_condition_stack.pop();

                if(!state->m_condition_stack.empty())
                {
                    const auto& newConditionState = state->m_condition_stack.top();
                    if (newConditionState.m_in_condition_elements)
                        state->m_current_nested_event_handler_set = newConditionState.m_condition->m_condition_elements.get();
                    else
                        state->m_current_nested_event_handler_set = newConditionState.m_condition->m_else_elements.get();
                }
                else
                    state->m_current_nested_event_handler_set = state->m_current_event_handler_set;
            }
            else
            {
                state->m_current_event_handler_set = nullptr;
                state->m_current_nested_event_handler_set = nullptr;
            }
        }
    };
}

using namespace event_handler_set_scope_sequences;

EventHandlerSetScopeSequences::EventHandlerSetScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void EventHandlerSetScopeSequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
}
