#include "StructuredDataStructScopeSequences.h"

#include <algorithm>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Utils/Alignment.h"

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

        static constexpr auto CAPTURE_STRING_LENGTH = 1;
        static constexpr auto CAPTURE_TYPE_NAME = 2;
        static constexpr auto CAPTURE_ENTRY_NAME = 3;
        static constexpr auto CAPTURE_ARRAY_SIZE = 4;

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
                    create.Integer().Capture(CAPTURE_ARRAY_SIZE),
                    create.Identifier().Capture(CAPTURE_ARRAY_SIZE)
                }),
                create.Char(']')
            });
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
        static CommonStructuredDataType ProcessType(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result)
        {
            const auto typeTag = result.NextTag();

            switch (typeTag)
            {
            case TAG_TYPE_INT:
                return CommonStructuredDataType(CommonStructuredDataTypeCategory::INT);
            case TAG_TYPE_BYTE:
                return CommonStructuredDataType(CommonStructuredDataTypeCategory::BYTE);
            case TAG_TYPE_BOOL:
                return CommonStructuredDataType(CommonStructuredDataTypeCategory::BOOL);
            case TAG_TYPE_FLOAT:
                return CommonStructuredDataType(CommonStructuredDataTypeCategory::FLOAT);
            case TAG_TYPE_SHORT:
                return CommonStructuredDataType(CommonStructuredDataTypeCategory::SHORT);
            case TAG_TYPE_STRING:
                {
                    const auto& stringLengthToken = result.NextCapture(CAPTURE_STRING_LENGTH);
                    const auto stringLength = stringLengthToken.IntegerValue();

                    if (stringLength <= 0)
                        throw ParsingException(stringLengthToken.GetPos(), "String length must be greater than zero");

                    return {CommonStructuredDataTypeCategory::STRING, static_cast<size_t>(stringLength)};
                }
            case TAG_TYPE_NAMED:
                {
                    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE_NAME);
                    const auto typeName = typeNameToken.IdentifierValue();

                    const auto existingType = state->m_def_types_by_name.find(typeName);
                    if (existingType == state->m_def_types_by_name.end())
                    {
                        const auto undefinedTypeIndex = state->m_undefined_types.size();
                        const CommonStructuredDataType undefinedType(CommonStructuredDataTypeCategory::UNKNOWN, undefinedTypeIndex);
                        state->m_undefined_types.emplace_back(typeName, typeNameToken.GetPos());
                        state->m_def_types_by_name.emplace(std::make_pair(typeName, undefinedType));
                        return undefinedType;
                    }

                    return existingType->second;
                }
            default:
                throw ParsingException(TokenPos(), "Invalid Tag for Type @ ProcessType!!!");
            }
        }

        static CommonStructuredDataType ProcessArray(StructuredDataDefParserState* state, const SimpleParserValue& arrayToken, const CommonStructuredDataType currentType)
        {
            if (arrayToken.m_type == SimpleParserValueType::INTEGER)
            {
                const auto arrayElementCount = arrayToken.IntegerValue();

                if (arrayElementCount <= 0)
                    throw ParsingException(arrayToken.GetPos(), "Array size must be greater than zero");

                const CommonStructuredDataIndexedArray indexedArray(currentType, arrayElementCount);

                const auto existingIndexedArray = state->m_def_indexed_arrays.find(indexedArray);
                if (existingIndexedArray != state->m_def_indexed_arrays.end())
                    return {CommonStructuredDataTypeCategory::INDEXED_ARRAY, existingIndexedArray->second};

                const auto newIndexedArrayIndex = state->m_current_def->m_indexed_arrays.size();
                state->m_current_def->m_indexed_arrays.push_back(indexedArray);
                return {CommonStructuredDataTypeCategory::INDEXED_ARRAY, newIndexedArrayIndex};
            }

            if (arrayToken.m_type == SimpleParserValueType::IDENTIFIER)
            {
                const auto enumName = arrayToken.IdentifierValue();

                const auto existingType = state->m_def_types_by_name.find(enumName);
                if (existingType == state->m_def_types_by_name.end())
                    throw ParsingException(arrayToken.GetPos(), "No type defined under this name");
                if (existingType->second.m_category != CommonStructuredDataTypeCategory::ENUM)
                    throw ParsingException(arrayToken.GetPos(), "Type for enumed array must be an enum");

                assert(existingType->second.m_info.type_index < state->m_current_def->m_enums.size());
                const auto* _enum = state->m_current_def->m_enums[existingType->second.m_info.type_index].get();

                const auto enumElementCount = _enum->ElementCount();
                assert(enumElementCount > 0);

                const CommonStructuredDataEnumedArray enumedArray(currentType, existingType->second.m_info.type_index);

                const auto existingEnumedArray = state->m_def_enumed_arrays.find(enumedArray);
                if (existingEnumedArray != state->m_def_enumed_arrays.end())
                    return {CommonStructuredDataTypeCategory::ENUM_ARRAY, existingEnumedArray->second};

                const auto newEnumedArrayIndex = state->m_current_def->m_enumed_arrays.size();
                state->m_current_def->m_enumed_arrays.push_back(enumedArray);
                return {CommonStructuredDataTypeCategory::ENUM_ARRAY, newEnumedArrayIndex};
            }

            throw ParsingException(arrayToken.GetPos(), "Invalid Token for Array @ ProcessArray!!!");
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def != nullptr);
            assert(state->m_current_struct != nullptr);

            auto currentType = ProcessType(state, result);

            std::vector<std::reference_wrapper<const SimpleParserValue>> arrayTokens;
            while (result.HasNextCapture(CAPTURE_ARRAY_SIZE))
                arrayTokens.emplace_back(result.NextCapture(CAPTURE_ARRAY_SIZE));

            for (auto i = arrayTokens.rbegin(); i != arrayTokens.rend(); ++i)
                currentType = ProcessArray(state, i->get(), currentType);

            state->m_current_struct->m_properties.emplace_back(result.NextCapture(CAPTURE_ENTRY_NAME).IdentifierValue(), currentType, state->m_current_struct_padding_offset);
        }
    };

    class SequencePadding final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_PADDING_VALUE = 1;

    public:
        SequencePadding()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("pad"),
                create.Char('('),
                create.Integer().Capture(CAPTURE_PADDING_VALUE),
                create.Char(')'),
                create.Char(';')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& paddingValueToken = result.NextCapture(CAPTURE_PADDING_VALUE);
            const auto paddingValue = paddingValueToken.IntegerValue();

            if (paddingValue <= 0)
                throw ParsingException(paddingValueToken.GetPos(), "Padding value must be greater than 0");

            state->m_current_struct_padding_offset += paddingValue * 8;
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

            state->m_current_struct->m_size_in_byte = utils::Align(state->m_current_struct_padding_offset, 8u) / 8;
            state->m_current_struct_padding_offset = 0u;

            // Sort the entries of the struct alphabetically
            state->m_current_struct->SortPropertiesByName();
            state->m_current_struct = nullptr;
        }
    };
}

using namespace sdd;
using namespace struct_scope_sequences;

StructuredDataStructScopeSequences::StructuredDataStructScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                       std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataStructScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceCloseStruct>());
    AddSequence(std::make_unique<SequencePadding>());
    AddSequence(std::make_unique<SequenceStructEntry>());
}
