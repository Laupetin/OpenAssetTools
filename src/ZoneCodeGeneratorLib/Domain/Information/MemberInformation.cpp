#include "MemberInformation.h"

MemberInformation::MemberInformation(StructureInformation* parent, StructureInformation* type, TypeInformation* typeInfo, Variable* member)
    : m_parent(parent),
      m_type(type),
      m_type_info(typeInfo),
      m_member(member),
      m_serialized_offset(0),
      m_serialized_type_size(0),
      m_is_string(false),
      m_is_script_string(false),
      m_is_reusable(false),
      m_pointer_array_elements_are_reusable(false),
      m_pointer_array_word_size(WordSize::UNKNOWN),
      m_is_leaf(false),
      m_fast_file_block(nullptr)
{
}
