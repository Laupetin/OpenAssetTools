#include "StructuredDataDefParserState.h"

using namespace sdd;

StructuredDataDefParserState::StructuredDataDefParserState()
    : m_current_def(nullptr),
      m_current_enum(nullptr),
      m_current_struct(nullptr)
{
}
