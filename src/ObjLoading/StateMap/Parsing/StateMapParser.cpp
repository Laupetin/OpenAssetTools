#include "StateMapParser.h"

#include "Matcher/StateMapExpressionMatchers.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace state_map;

namespace state_map
{
    class SequenceStateMapEntry final : public StateMapParser::sequence_t
    {
        static constexpr auto CAPTURE_ENTRY_NAME = 1;

    public:
        SequenceStateMapEntry()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Identifier().Capture(CAPTURE_ENTRY_NAME),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& entryNameToken = result.NextCapture(CAPTURE_ENTRY_NAME);

            const auto foundEntry = state->m_valid_state_map_entry_names.find(entryNameToken.IdentifierValue());
            if (foundEntry == state->m_valid_state_map_entry_names.end())
                throw ParsingException(entryNameToken.GetPos(), "Unknown entry name");

            state->m_in_entry = true;
            state->m_current_entry_index = foundEntry->second;
        }
    };

    class SequenceStateMapEntryClose final : public StateMapParser::sequence_t
    {
    public:
        SequenceStateMapEntryClose()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}')
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_in_entry = false;
        }
    };

    class SequenceCondition final : public StateMapParser::sequence_t
    {
        StateMapExpressionMatchers m_expression_matchers;

    public:
        SequenceCondition()
        {
            AddLabeledMatchers(m_expression_matchers.Expression(this), StateMapExpressionMatchers::LABEL_EXPRESSION);
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Label(StateMapExpressionMatchers::LABEL_EXPRESSION),
                create.Char(':')
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_entry);

            auto expression = m_expression_matchers.ProcessExpression(result);

            if (!state->m_current_rule)
            {
                auto newRule = std::make_unique<StateMapRule>();
                state->m_current_rule = newRule.get();
                state->m_definition->m_state_map_entries[state->m_current_entry_index].m_rules.emplace_back(std::move(newRule));
            }

            state->m_current_rule->m_conditions.emplace_back(std::move(expression));
        }
    };

    class SequenceValue final : public StateMapParser::sequence_t
    {
        static constexpr auto TAG_PASSTHROUGH = 1;
        static constexpr auto TAG_VALUE_LIST = 2;

        static constexpr auto CAPTURE_VALUE = 1;

        static constexpr auto LABEL_VALUE_LIST = 1;

    public:
        SequenceValue()
        {
            const SimpleMatcherFactory create(this);

            AddLabeledMatchers({
                                   create.Identifier().Capture(CAPTURE_VALUE),
                                   create.OptionalLoop(create.And({
                                       create.Char(','),
                                       create.Identifier().Capture(CAPTURE_VALUE)
                                   }))
                               }, LABEL_VALUE_LIST);

            AddMatchers({
                create.Or({
                    create.Keyword("passthrough").Tag(TAG_PASSTHROUGH),
                    create.Label(LABEL_VALUE_LIST).Tag(TAG_VALUE_LIST)
                }),
                create.Char(';')
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_entry);
            assert(state->m_current_rule);

            if (result.PeekAndRemoveIfTag(TAG_VALUE_LIST) == TAG_VALUE_LIST)
            {
                while (result.HasNextCapture(CAPTURE_VALUE))
                    state->m_current_rule->m_values.emplace_back(result.NextCapture(CAPTURE_VALUE).IdentifierValue());
            }
            else
            {
                // A rule without values is considered passthrough therefore just don't add values
                assert(result.PeekAndRemoveIfTag(TAG_PASSTHROUGH) == TAG_PASSTHROUGH);
            }

            state->m_current_rule = nullptr;
        }
    };
}

StateMapParser::StateMapParser(SimpleLexer* lexer, std::string stateMapName, const StateMapLayout& layout)
    : AbstractParser(lexer, std::make_unique<StateMapParserState>(std::move(stateMapName), layout))
{
}

const std::vector<StateMapParser::sequence_t*>& StateMapParser::GetTestsForState()
{
    static std::vector<sequence_t*> rootSequences({
        new SequenceStateMapEntry()
    });

    static std::vector<sequence_t*> entrySequences({
        new SequenceStateMapEntryClose(),
        new SequenceCondition()
    });

    static std::vector<sequence_t*> ruleSequences({
        new SequenceCondition(),
        new SequenceValue()
    });

    if (m_state->m_in_entry)
    {
        return m_state->m_current_rule
                   ? ruleSequences
                   : entrySequences;
    }

    return rootSequences;
}

std::unique_ptr<StateMapDefinition> StateMapParser::GetStateMapDefinition() const
{
    return std::move(m_state->m_definition);
}

StateMapParserState* StateMapParser::GetState() const
{
    return m_state.get();
}
