#include "EventHandlerSetScopeSequences.h"

#include <sstream>

#include "Generic/GenericStringPropertySequence.h"
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
            state->m_current_event_handler_set = nullptr;
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
