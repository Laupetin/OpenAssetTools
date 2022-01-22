#include "StructuredDataDefScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

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
                    create.Char(')')
                })),
                create.Identifier().Capture(CAPTURE_NAME),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            auto newEnum = std::make_unique<CommonStructuredDataEnum>(result.NextCapture(CAPTURE_NAME).IdentifierValue());
            if (result.HasNextCapture(CAPTURE_RESERVED_COUNT))
            {
                const auto& reservedCountToken = result.NextCapture(CAPTURE_RESERVED_COUNT);
                newEnum->m_reserved_entry_count = reservedCountToken.IntegerValue();

                if (newEnum->m_reserved_entry_count <= 0)
                    throw ParsingException(reservedCountToken.GetPos(), "Reserved enum entry count must be greater than zero");
            }

            state->m_current_enum = newEnum.get();
            state->m_def_types_by_name.emplace(newEnum->m_name, CommonStructuredDataType(CommonStructuredDataTypeCategory::ENUM, state->m_current_def->m_enums.size()));
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
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(StructuredDataDefParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_def);

            auto newStruct = std::make_unique<CommonStructuredDataStruct>(result.NextCapture(CAPTURE_NAME).IdentifierValue());
            auto* newStructPtr = newStruct.get();
            const auto newStructIndex = state->m_current_def->m_structs.size();

            state->m_current_struct = newStructPtr;
            state->m_def_types_by_name.emplace(newStruct->m_name, CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, newStructIndex));
            state->m_current_def->m_structs.emplace_back(std::move(newStruct));
            state->m_current_struct_offset_in_bits = 0;

            if (newStructPtr->m_name == "root")
            {
                state->m_current_struct_offset_in_bits = 64u;
                state->m_current_def->m_root_type = CommonStructuredDataType(CommonStructuredDataTypeCategory::STRUCT, newStructIndex);
            }
            else
                state->m_current_struct_offset_in_bits = 0;
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
                create.Char('}')
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
            SetDefSizeFromRootStruct(state);

            state->m_current_def = nullptr;
            state->m_def_types_by_name.clear();
            state->m_def_indexed_arrays.clear();
            state->m_def_enumed_arrays.clear();
        }
    };
}

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
}
