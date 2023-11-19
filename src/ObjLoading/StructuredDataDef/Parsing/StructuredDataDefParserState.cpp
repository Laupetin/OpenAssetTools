#include "StructuredDataDefParserState.h"

using namespace sdd;

UndefinedType::UndefinedType() = default;

UndefinedType::UndefinedType(std::string name, const TokenPos firstUsagePos)
    : m_name(std::move(name)),
      m_first_usage_pos(firstUsagePos)
{
}

StructuredDataDefParserState::StructuredDataDefParserState()
    : m_current_def(nullptr),
      m_current_enum(nullptr),
      m_current_struct(nullptr),
      m_current_struct_padding_offset(0u),
      m_checksum_override_value(0u),
      m_checksum_overriden(false)
{
}
