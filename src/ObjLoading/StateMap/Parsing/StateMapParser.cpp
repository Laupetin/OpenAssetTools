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
                create.Char('{'),
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
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

    public:
        SequenceStateMapEntryClose()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}').Capture(CAPTURE_FIRST_TOKEN),
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            if (!state->m_entry_has_default)
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Entry must have a default case");

            state->m_in_entry = false;
            state->m_entry_has_default = false;
        }
    };

    class SequenceCondition final : public StateMapParser::sequence_t
    {
        static constexpr auto TAG_DEFAULT = 1;
        static constexpr auto TAG_EXPRESSION = 2;

    public:
        SequenceCondition()
        {
            AddLabeledMatchers(StateMapExpressionMatchers().Expression(this), StateMapExpressionMatchers::LABEL_EXPRESSION);
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Or({
                    create.Keyword("default").Tag(TAG_DEFAULT),
                    create.Label(StateMapExpressionMatchers::LABEL_EXPRESSION).Tag(TAG_EXPRESSION),
                }),
                create.Char(':'),
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_definition);
            assert(state->m_in_entry);

            if (!state->m_current_rule)
            {
                auto newRule = std::make_unique<StateMapRule>();
                state->m_current_rule = newRule.get();
                state->m_definition->m_state_map_entries[state->m_current_entry_index].m_rules.emplace_back(std::move(newRule));
            }

            if (result.PeekAndRemoveIfTag(TAG_EXPRESSION) == TAG_EXPRESSION)
            {
                auto expression = StateMapExpressionMatchers(state).ProcessExpression(result);

                state->m_current_rule->m_conditions.emplace_back(std::move(expression));
            }
            else
            {
                assert(result.PeekAndRemoveIfTag(TAG_DEFAULT) == TAG_DEFAULT);
                auto& entry = state->m_definition->m_state_map_entries[state->m_current_entry_index];
                state->m_entry_has_default = true;
                entry.m_default_index = entry.m_rules.size() - 1;
            }
        }
    };

    class SequenceValue final : public StateMapParser::sequence_t
    {
        static constexpr auto TAG_PASSTHROUGH = 1;
        static constexpr auto TAG_VALUE_LIST = 2;

        static constexpr auto CAPTURE_VALUE = 1;
        static constexpr auto CAPTURE_VALUE_END = 2;

        static constexpr auto LABEL_VALUE_LIST = 1;
        static constexpr auto LABEL_VALUE = 2;

    public:
        SequenceValue()
        {
            const SimpleMatcherFactory create(this);

            AddLabeledMatchers(create.Or({
                                   create.Identifier(),
                                   create.Integer(),
                               }),
                               LABEL_VALUE);

            AddLabeledMatchers(
                {
                    create.Label(LABEL_VALUE).Capture(CAPTURE_VALUE),
                    create.OptionalLoop(create.And({
                        create.Char(','),
                        create.Label(LABEL_VALUE).Capture(CAPTURE_VALUE),
                    })),
                },
                LABEL_VALUE_LIST);

            AddMatchers({
                create.Or({
                    create.Keyword("passthrough").Tag(TAG_PASSTHROUGH),
                    create.Label(LABEL_VALUE_LIST).Tag(TAG_VALUE_LIST),
                }),
                create.Char(';').Capture(CAPTURE_VALUE_END),
            });
        }

    protected:
        void ProcessMatch(StateMapParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_entry);
            assert(state->m_current_rule);

            const auto& layoutEntry = state->m_layout.m_entry_layout.m_entries[state->m_current_entry_index];

            if (result.PeekAndRemoveIfTag(TAG_VALUE_LIST) == TAG_VALUE_LIST)
            {
                auto resultIndex = 0u;

                while (result.HasNextCapture(CAPTURE_VALUE))
                {
                    const auto& valueToken = result.NextCapture(CAPTURE_VALUE);

                    if (resultIndex >= layoutEntry.m_result_vars.size())
                        throw ParsingException(valueToken.GetPos(), "Too many results");

                    const auto& resultVar = layoutEntry.m_result_vars[resultIndex];
                    const auto varForResult = state->m_valid_vars.find(resultVar);

                    if (varForResult != state->m_valid_vars.end())
                    {
                        const auto& var = state->m_layout.m_var_layout.m_vars[varForResult->second];
                        const auto tokenValue =
                            valueToken.m_type == SimpleParserValueType::IDENTIFIER ? valueToken.IdentifierValue() : std::to_string(valueToken.IntegerValue());

                        const auto referencedValue = std::ranges::find_if(var.m_values,
                                                                          [&tokenValue](const StateMapLayoutVarValue& value)
                                                                          {
                                                                              return value.m_name == tokenValue;
                                                                          });

                        if (referencedValue == var.m_values.end())
                            throw ParsingException(valueToken.GetPos(), "Not part of the valid values for this var");

                        state->m_current_rule->m_value |= referencedValue->m_state_bits_mask;
                    }
                    else
                        throw ParsingException(valueToken.GetPos(), "Unknown var for result?");

                    resultIndex++;
                }

                if (resultIndex < layoutEntry.m_result_vars.size())
                    throw ParsingException(result.NextCapture(CAPTURE_VALUE_END).GetPos(), "Not enough results");
            }
            else
            {
                // A rule without values is considered passthrough therefore just don't add values
                assert(result.PeekAndRemoveIfTag(TAG_PASSTHROUGH) == TAG_PASSTHROUGH);
                state->m_current_rule->m_passthrough = true;
            }

            state->m_current_rule = nullptr;
        }
    };
} // namespace state_map

StateMapParser::StateMapParser(SimpleLexer* lexer, std::string stateMapName, const StateMapLayout& layout)
    : AbstractParser(lexer, std::make_unique<StateMapParserState>(std::move(stateMapName), layout))
{
}

const std::vector<StateMapParser::sequence_t*>& StateMapParser::GetTestsForState()
{
    static std::vector<sequence_t*> rootSequences({
        new SequenceStateMapEntry(),
    });

    static std::vector<sequence_t*> entrySequences({
        new SequenceStateMapEntryClose(),
        new SequenceCondition(),
    });

    static std::vector<sequence_t*> ruleSequences({
        new SequenceCondition(),
        new SequenceValue(),
    });

    if (m_state->m_in_entry)
    {
        return m_state->m_current_rule ? ruleSequences : entrySequences;
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
