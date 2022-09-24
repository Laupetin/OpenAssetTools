#include "StateMapParser.h"

#include <array>
#include <sstream>

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
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

    public:
        SequenceStateMapEntryClose()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}').Capture(CAPTURE_FIRST_TOKEN)
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
        static constexpr auto TAG_CONDITION = 2;
        static constexpr auto TAG_EQUALS_OPERATION = 3;

        static constexpr auto CAPTURE_OPERATOR = 1;

        static constexpr auto LABEL_CONDITION = 1;
        static constexpr auto LABEL_EQUALS_OPERATION = 2;
        static constexpr auto LABEL_CONDITION_OPERATOR = 3;

    public:
        SequenceCondition()
        {
            const SimpleMatcherFactory create(this);

            AddLabeledMatchers(create.Or({
                                   create.Identifier(),
                                   create.Integer()
                               }).Capture(CAPTURE_OPERATOR), LABEL_CONDITION_OPERATOR);

            AddLabeledMatchers(create.And({
                                   create.True().Tag(TAG_EQUALS_OPERATION),
                                   create.Label(LABEL_CONDITION_OPERATOR),
                                   create.MultiChar(StateMapParser::MULTI_TOKEN_EQUALS),
                                   create.Label(LABEL_CONDITION_OPERATOR)
                               }), LABEL_EQUALS_OPERATION);

            AddLabeledMatchers(create.And({
                                   create.True().Tag(TAG_CONDITION),
                                   create.Label(LABEL_EQUALS_OPERATION),
                                   create.OptionalLoop(create.And({
                                       create.MultiChar(StateMapParser::MULTI_TOKEN_AND),
                                       create.Label(LABEL_EQUALS_OPERATION)
                                   }))
                               }), LABEL_CONDITION);

            AddMatchers({
                create.Or({
                    create.Keyword("default").Tag(TAG_DEFAULT),
                    create.Label(LABEL_CONDITION)
                }),
                create.Char(':')
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

            if (result.PeekAndRemoveIfTag(TAG_CONDITION) == TAG_CONDITION)
            {
                auto condition = ProcessCondition(state, result);
                state->m_current_rule->m_conditions.emplace_back(std::move(condition));
            }
            else
            {
                assert(result.PeekAndRemoveIfTag(TAG_DEFAULT) == TAG_DEFAULT);
                auto& entry = state->m_definition->m_state_map_entries[state->m_current_entry_index];
                state->m_entry_has_default = true;
                entry.m_default_index = entry.m_rules.size() - 1;
            }
        }

        static StateMapCondition ProcessCondition(const StateMapParserState* state, SequenceResult<SimpleParserValue>& result)
        {
            StateMapCondition condition(state->m_layout.m_state_bits_count);

            while (result.PeekAndRemoveIfTag(TAG_EQUALS_OPERATION) == TAG_EQUALS_OPERATION)
            {
                std::array<std::reference_wrapper<const SimpleParserValue>, 2> operatorToken
                {
                    result.NextCapture(CAPTURE_OPERATOR),
                    result.NextCapture(CAPTURE_OPERATOR)
                };

                std::array<std::string, 2> operatorValue
                {
                    ProcessOperator(operatorToken[0]),
                    ProcessOperator(operatorToken[1])
                };

                const auto variableIndex = IsVariable(operatorValue[1]) ? 1 : 0;
                const auto valueIndex = variableIndex == 0 ? 1 : 0;

                const auto& variable = GetVariable(state, operatorToken[variableIndex], operatorValue[variableIndex]);
                const auto& value = GetValue(variable, operatorToken[valueIndex], operatorValue[valueIndex]);

                condition.m_masks_per_index[variable.m_state_bits_index] |= variable.m_values_mask;
                condition.m_values_per_index[variable.m_state_bits_index] |= value.m_state_bits_mask;
            }

            return condition;
        }

        static std::string ProcessOperator(const SimpleParserValue& token)
        {
            return token.m_type == SimpleParserValueType::IDENTIFIER ? token.IdentifierValue() : std::to_string(token.IntegerValue());
        }

        static bool IsVariable(const std::string& _operator)
        {
            return _operator.rfind("mtl", 0) == 0;
        }

        static const StateMapLayoutVar& GetVariable(const StateMapParserState* state, const SimpleParserValue& operatorToken, const std::string& operatorValue)
        {
            const auto& foundVariable = state->m_valid_vars.find(operatorValue);
            if (foundVariable == state->m_valid_vars.end())
                throw ParsingException(operatorToken.GetPos(), "Unknown variable");

            return state->m_layout.m_var_layout.m_vars[foundVariable->second];
        }

        static const StateMapLayoutVarValue& GetValue(const StateMapLayoutVar& variable, const SimpleParserValue& operatorToken, const std::string& operatorValue)
        {
            const auto matchingValue = std::find_if(variable.m_values.begin(), variable.m_values.end(), [&operatorValue](const StateMapLayoutVarValue& value)
            {
                return value.m_name == operatorValue;
            });

            if (matchingValue == variable.m_values.end())
            {
                std::ostringstream ss;
                ss << "Unknown value for variable \"" << variable.m_name << "\"";
                throw ParsingException(operatorToken.GetPos(), ss.str());
            }

            return *matchingValue;
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
                                   create.Integer()
                               }), LABEL_VALUE);

            AddLabeledMatchers({
                                   create.Label(LABEL_VALUE).Capture(CAPTURE_VALUE),
                                   create.OptionalLoop(create.And({
                                       create.Char(','),
                                       create.Label(LABEL_VALUE).Capture(CAPTURE_VALUE)
                                   }))
                               }, LABEL_VALUE_LIST);

            AddMatchers({
                create.Or({
                    create.Keyword("passthrough").Tag(TAG_PASSTHROUGH),
                    create.Label(LABEL_VALUE_LIST).Tag(TAG_VALUE_LIST)
                }),
                create.Char(';').Capture(CAPTURE_VALUE_END)
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
                        const auto tokenValue = valueToken.m_type == SimpleParserValueType::IDENTIFIER ? valueToken.IdentifierValue() : std::to_string(valueToken.IntegerValue());

                        const auto referencedValue = std::find_if(var.m_values.begin(), var.m_values.end(), [&tokenValue](const StateMapLayoutVarValue& value)
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
