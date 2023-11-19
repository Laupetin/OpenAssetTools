#include "StructuredDataDefScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "StructuredDataDef/Parsing/StructuredDataDefSizeCalculator.h"

#include <sstream>

namespace sdd::def_scope_sequences
{
    class SequenceEnum final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;
        static constexpr auto CAPTURE_RESERVED_COUNT = 2;

    public:
        SequenceEnum()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("enum"),
                create.Optional(create.And({
                    create.Char('('),
                    create.Integer().Capture(CAPTURE_RESERVED_COUNT),
                    create.Char(')'),
                })),
                create.Identifier().Capture(CAPTURE_NAME),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            const auto& nameToken = result.NextCapture(CAPTURE_NAME);
            auto newEnum = std::make_unique<CommonStructuredDataEnum>(nameToken.IdentifierValue());
            const auto newEnumIndex = state->m_current_def->m_enums.size();
            if (result.HasNextCapture(CAPTURE_RESERVED_COUNT))
            {
                const auto& reservedCountToken = result.NextCapture(CAPTURE_RESERVED_COUNT);
                newEnum->m_reserved_entry_count = reservedCountToken.IntegerValue();

                if (newEnum->m_reserved_entry_count <= 0)
                    throw ParsingException(reservedCountToken.GetPos(), "Reserved enum entry count must be greater than zero");
            }

            const auto existingType = state->m_def_types_by_name.find(newEnum->m_name);
            if (existingType != state->m_def_types_by_name.end())
            {
                if (existingType->second.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                {
                    assert(existingType->second.m_info.type_index < state->m_undefined_types.size());
                    auto& undefinedType = state->m_undefined_types[existingType->second.m_info.type_index];
                    undefinedType.m_mapped_type = CommonStructuredDataType(CommonStructuredDataTypeCategory::ENUM, newEnumIndex);
                    existingType->second = undefinedType.m_mapped_type;
                }
                else
                    throw ParsingException(nameToken.GetPos(), "Type with this name has already been defined");
            }
            else
                state->m_def_types_by_name.emplace(newEnum->m_name, CommonStructuredDataType(CommonStructuredDataTypeCategory::ENUM, newEnumIndex));

            state->m_current_enum = newEnum.get();
            state->m_current_def->m_enums.emplace_back(std::move(newEnum));
        }
    };

    class SequenceStruct final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceStruct()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("struct"),
                create.Identifier().Capture(CAPTURE_NAME),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            const auto& nameToken = result.NextCapture(CAPTURE_NAME);
            auto newStruct = std::make_unique<CommonStructuredDataStruct>(nameToken.IdentifierValue());
            auto* newStructPtr = newStruct.get();
            const auto newStructIndex = state->m_current_def->m_structs.size();

            const auto existingType = state->m_def_types_by_name.find(newStruct->m_name);
            if (existingType != state->m_def_types_by_name.end())
            {
                if (existingType->second.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                {
                    assert(existingType->second.m_info.type_index < state->m_undefined_types.size());
                    auto& undefinedType = state->m_undefined_types[existingType->second.m_info.type_index];
                    undefinedType.m_mapped_type = CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, newStructIndex);
                    existingType->second = undefinedType.m_mapped_type;
                }
                else
                    throw ParsingException(nameToken.GetPos(), "Type with this name has already been defined");
            }
            else
                state->m_def_types_by_name.emplace(newStruct->m_name, CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, newStructIndex));

            state->m_current_struct = newStructPtr;
            state->m_current_def->m_structs.emplace_back(std::move(newStruct));
            state->m_current_struct_padding_offset = 0;

            if (newStructPtr->m_name == "root")
            {
                state->m_current_struct_padding_offset = 64u;
                state->m_current_struct_is_root = true;
                state->m_current_def->m_root_type = CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, newStructIndex);
            }
            else
            {
                state->m_current_struct_is_root = false;
                state->m_current_struct_padding_offset = 0;
            }
        }
    };

    class SequenceChecksumOverride final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_VALUE = 1;

    public:
        SequenceChecksumOverride()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("checksumoverride"),
                create.Integer().Capture(CAPTURE_VALUE),
                create.Char(';'),
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            state->m_checksum_overriden = true;
            state->m_checksum_override_value = result.NextCapture(CAPTURE_VALUE).IntegerValue();
        }
    };

    class SequenceCloseScope final : public StructuredDataDefParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceCloseScope()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}'),
            });
        }

    private:
        static void CreateDefaultStructWhenNoStructsSpecified(const StructuredDataDefParserState* state)
        {
            if (state->m_current_def->m_structs.empty())
            {
                state->m_current_def->m_structs.emplace_back(std::make_unique<CommonStructuredDataStruct>());
                state->m_current_def->m_root_type = CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, 0u);
            }
        }

        static void EnsureAllUsedTypesHaveBeenDefined(const StructuredDataDefParserState* state)
        {
            for (const auto& undefinedType : state->m_undefined_types)
            {
                if (undefinedType.m_mapped_type.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                {
                    std::ostringstream ss;
                    ss << "Type \"" << undefinedType.m_name << "\" has not been defined";
                    throw ParsingException(undefinedType.m_first_usage_pos, ss.str());
                }
            }
        }

        static void ReplaceUndefinedTypeReference(const StructuredDataDefParserState* state)
        {
            auto& def = *state->m_current_def;

            for (const auto& _struct : def.m_structs)
            {
                for (auto& property : _struct->m_properties)
                {
                    if (property.m_type.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                    {
                        assert(property.m_type.m_info.type_index < state->m_undefined_types.size());
                        const auto& undefinedType = state->m_undefined_types[property.m_type.m_info.type_index];
                        property.m_type = undefinedType.m_mapped_type;
                    }
                }
            }

            for (auto& indexedArray : def.m_indexed_arrays)
            {
                if (indexedArray.m_array_type.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                {
                    assert(indexedArray.m_array_type.m_info.type_index < state->m_undefined_types.size());
                    const auto& undefinedType = state->m_undefined_types[indexedArray.m_array_type.m_info.type_index];
                    indexedArray.m_array_type = undefinedType.m_mapped_type;
                }
            }

            for (auto& enumedArray : def.m_enumed_arrays)
            {
                if (enumedArray.m_array_type.m_category == CommonStructuredDataTypeCategory::UNKNOWN)
                {
                    assert(enumedArray.m_array_type.m_info.type_index < state->m_undefined_types.size());
                    const auto& undefinedType = state->m_undefined_types[enumedArray.m_array_type.m_info.type_index];
                    enumedArray.m_array_type = undefinedType.m_mapped_type;
                }
            }
        }

        static void CalculateSizesAndOffsetsForDef(const StructuredDataDefParserState* state)
        {
            try
            {
                StructuredDataDefSizeCalculator::CalculateSizesAndOffsetsForDef(*state->m_current_def);
            }
            catch (SizeCalculationException& e)
            {
                throw ParsingException(TokenPos(), e.Message());
            }
        }

        static void SetDefSizeFromRootStruct(const StructuredDataDefParserState* state)
        {
            if (state->m_current_def->m_root_type.m_category == CommonStructuredDataTypeCategory::STRUCT
                && state->m_current_def->m_root_type.m_info.type_index < state->m_current_def->m_structs.size())
            {
                const auto* _struct = state->m_current_def->m_structs[state->m_current_def->m_root_type.m_info.type_index].get();
                state->m_current_def->m_size_in_byte = _struct->m_size_in_byte;
            }
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_enum == nullptr);
            assert(state->m_current_struct == nullptr);

            CreateDefaultStructWhenNoStructsSpecified(state);
            EnsureAllUsedTypesHaveBeenDefined(state);
            ReplaceUndefinedTypeReference(state);
            CalculateSizesAndOffsetsForDef(state);
            SetDefSizeFromRootStruct(state);

            if (!state->m_checksum_overriden)
                state->m_current_def->m_checksum = state->m_current_def->CalculateChecksum();
            else
                state->m_current_def->m_checksum = state->m_checksum_override_value;

            state->m_current_def = nullptr;
            state->m_def_types_by_name.clear();
            state->m_def_indexed_arrays.clear();
            state->m_def_enumed_arrays.clear();
            state->m_checksum_overriden = false;
            state->m_checksum_override_value = 0u;
        }
    };
} // namespace sdd::def_scope_sequences

using namespace sdd;
using namespace def_scope_sequences;

StructuredDataDefScopeSequences::StructuredDataDefScopeSequences(std::vector<std::unique_ptr<StructuredDataDefParser::sequence_t>>& allSequences,
                                                                 std::vector<StructuredDataDefParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void StructuredDataDefScopeSequences::AddSequences() const
{
    AddSequence(std::make_unique<SequenceCloseScope>());
    AddSequence(std::make_unique<SequenceEnum>());
    AddSequence(std::make_unique<SequenceStruct>());
    AddSequence(std::make_unique<SequenceChecksumOverride>());
}
