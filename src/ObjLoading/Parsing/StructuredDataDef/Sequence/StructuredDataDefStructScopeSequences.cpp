#include "StructuredDataDefStructScopeSequences.h"

#include <algorithm>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace sdd::struct_scope_sequences
{
    class SequenceStructEntry final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto TAG_TYPE_INT = 1;
        static constexpr auto TAG_TYPE_BYTE = 2;
        static constexpr auto TAG_TYPE_BOOL = 3;
        static constexpr auto TAG_TYPE_FLOAT = 4;
        static constexpr auto TAG_TYPE_SHORT = 5;
        static constexpr auto TAG_TYPE_STRING = 6;
        static constexpr auto TAG_TYPE_NAMED = 7;
        static constexpr auto TAG_ARRAY = 8;
        static constexpr auto TAG_ARRAY_INDEX = 9;
        static constexpr auto TAG_ARRAY_ENUM = 10;

        static constexpr auto CAPTURE_STRING_LENGTH = 1;
        static constexpr auto CAPTURE_TYPE_NAME = 2;
        static constexpr auto CAPTURE_ENTRY_NAME = 3;
        static constexpr auto CAPTURE_ARRAY_SIZE = 4;
        static constexpr auto CAPTURE_ARRAY_ENUM = 5;

        static std::unique_ptr<matcher_t> TypeMatchers(const SimpleMatcherFactory& create)
        {
            return create.Or({
                create.Keyword("int").Tag(TAG_TYPE_INT),
                create.Keyword("byte").Tag(TAG_TYPE_BYTE),
                create.Keyword("bool").Tag(TAG_TYPE_BOOL),
                create.Keyword("float").Tag(TAG_TYPE_FLOAT),
                create.Keyword("short").Tag(TAG_TYPE_SHORT),
                create.And({
                    create.Keyword("string"),
                    create.Char('('),
                    create.Integer().Capture(CAPTURE_STRING_LENGTH),
                    create.Char(')')
                }).Tag(TAG_TYPE_STRING),
                create.Identifier().Tag(TAG_TYPE_NAMED).Capture(CAPTURE_TYPE_NAME)
            });
        }

        static std::unique_ptr<matcher_t> ArrayMatchers(const SimpleMatcherFactory& create)
        {
            return create.And({
                create.Char('['),
                create.Or({
                    create.Integer().Tag(TAG_ARRAY_INDEX).Capture(CAPTURE_ARRAY_SIZE),
                    create.Identifier().Tag(TAG_ARRAY_ENUM).Capture(CAPTURE_ARRAY_ENUM)
                }),
                create.Char(']')
            }).Tag(TAG_ARRAY);
        }

    public:
        SequenceStructEntry()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                TypeMatchers(create),
                create.Identifier().Capture(CAPTURE_ENTRY_NAME),
                create.OptionalLoop(ArrayMatchers(create)),
                create.Char(';')
            });
        }

    private:
        static CommonStructuredDataDefType ProcessType(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result)
        {
            const auto typeTag = result.NextTag();

            switch (typeTag)
            {
            case TAG_TYPE_INT:
                return CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory::INT);
            case TAG_TYPE_BYTE:
                return CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory::BYTE);
            case TAG_TYPE_BOOL:
                return CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory::BOOL);
            case TAG_TYPE_FLOAT:
                return CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory::FLOAT);
            case TAG_TYPE_SHORT:
                return CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory::SHORT);
            case TAG_TYPE_STRING:
                {
                    const auto& stringLengthToken = result.NextCapture(CAPTURE_STRING_LENGTH);
                    const auto stringLength = stringLengthToken.IntegerValue();

                    if (stringLength <= 0)
                        throw ParsingException(stringLengthToken.GetPos(), "String length must be greater than zero");

                    return {CommonStructuredDataDefTypeCategory::STRING, static_cast<size_t>(stringLength)};
                }
            case TAG_TYPE_NAMED:
                {
                    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE_NAME);
                    const auto typeName = typeNameToken.IdentifierValue();

                    const auto existingType = state->m_def_types_by_name.find(typeName);
                    if (existingType == state->m_def_types_by_name.end())
                        throw ParsingException(typeNameToken.GetPos(), "No type defined under this name");

                    return existingType->second;
                }
            default:
                throw ParsingException(TokenPos(), "Invalid Tag for Type @ ProcessType!!!");
            }
        }

        static CommonStructuredDataDefType ProcessArray(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result, CommonStructuredDataDefType currentType)
        {
            const auto arrayTag = result.NextTag();

            if (arrayTag == TAG_ARRAY_INDEX)
            {
                const auto& arraySizeToken = result.NextCapture(CAPTURE_ARRAY_SIZE);
                const auto arraySize = arraySizeToken.IntegerValue();

                if (arraySize <= 0)
                    throw ParsingException(arraySizeToken.GetPos(), "Array size must be greater than zero");

                const CommonStructuredDataDefIndexedArray indexedArray(currentType, arraySize);

                const auto existingIndexedArray = state->m_def_indexed_arrays.find(indexedArray);
                if (existingIndexedArray != state->m_def_indexed_arrays.end())
                    return {CommonStructuredDataDefTypeCategory::INDEXED_ARRAY, existingIndexedArray->second};

                const auto newIndexedArrayIndex = state->m_current_def->m_indexed_arrays.size();
                state->m_current_def->m_indexed_arrays.push_back(indexedArray);
                return {CommonStructuredDataDefTypeCategory::INDEXED_ARRAY, newIndexedArrayIndex};
            }

            if (arrayTag == TAG_ARRAY_ENUM)
            {
                const auto& enumNameToken = result.NextCapture(CAPTURE_ARRAY_ENUM);
                const auto enumName = enumNameToken.IdentifierValue();

                const auto existingType = state->m_def_types_by_name.find(enumName);
                if (existingType == state->m_def_types_by_name.end())
                    throw ParsingException(enumNameToken.GetPos(), "No type defined under this name");
                if (existingType->second.m_category != CommonStructuredDataDefTypeCategory::ENUM)
                    throw ParsingException(enumNameToken.GetPos(), "Type for enumed array must be an enum");

                const CommonStructuredDataDefEnumedArray enumedArray(currentType, existingType->second.m_info.type_index);

                const auto existingEnumedArray = state->m_def_enumed_arrays.find(enumedArray);
                if (existingEnumedArray != state->m_def_enumed_arrays.end())
                    return {CommonStructuredDataDefTypeCategory::ENUM_ARRAY, existingEnumedArray->second};

                const auto newEnumedArrayIndex = state->m_current_def->m_enumed_arrays.size();
                state->m_current_def->m_enumed_arrays.push_back(enumedArray);
                return {CommonStructuredDataDefTypeCategory::ENUM_ARRAY, newEnumedArrayIndex};
            }

            throw ParsingException(TokenPos(), "Invalid Tag for Array @ ProcessArray!!!");
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def != nullptr);
            assert(state->m_current_struct != nullptr);

            auto currentType = ProcessType(state, result);

            while (result.PeekAndRemoveIfTag(TAG_ARRAY) == TAG_ARRAY)
                currentType = ProcessArray(state, result, currentType);

            // TODO: Calculate offset
            state->m_current_struct->m_properties.emplace_back(result.NextCapture(CAPTURE_ENTRY_NAME).IdentifierValue(), currentType, 0);
        }
    };

    class SequenceCloseStruct final : public StructuredDataDefParser::sequence_t
    {
    public:
        SequenceCloseStruct()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}'),
                create.Optional(create.Char(';'))
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_struct != nullptr);

            std::sort(state->m_current_struct->m_properties.begin(), state->m_current_struct->m_properties.end(),
                      [](const CommonStructuredDataDefStructEntry& e1, const CommonStructuredDataDefStructEntry& e2)
                      {
                          return e1.m_name < e2.m_name;
                      });
            state->m_current_struct = nullptr;
        }
    };
}

using namespace sdd;
using namespace struct_scope_sequences;

StructuredDataDefStructScopeSequences::StructuredDataDefStructScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                             std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataDefStructScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceCloseStruct>());
    AddSequence(std::make_unique<SequenceStructEntry>());
}
