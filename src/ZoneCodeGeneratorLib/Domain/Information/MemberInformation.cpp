#include "MemberInformation.h"

MemberInformation::MemberInformation(StructureInformation* parent, StructureInformation* type, Variable* member)
    : m_parent(parent),
      m_type(type),
      m_member(member),
      m_is_string(false),
      m_is_script_string(false),
      m_is_reusable(false),
      m_is_leaf(false),
      m_fast_file_block(nullptr),
      m_asset_ref(nullptr)
{
}
