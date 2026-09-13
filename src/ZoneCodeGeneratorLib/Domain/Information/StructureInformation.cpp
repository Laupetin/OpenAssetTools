#include "StructureInformation.h"

StructureInformation::StructureInformation(DefinitionWithMembers* definition, TypeInformation* typeInfo)
    : m_definition(definition),
      m_type_info(typeInfo),
      m_word_size(WordSize::UNKNOWN),
      m_alias_word_size(WordSize::UNKNOWN),
      m_serialized_word_size(WordSize::UNKNOWN),
      m_serialized_size(0),
      m_is_leaf(false),
      m_requires_marking(false),
      m_has_matching_cross_platform_structure(false),
      m_embedded_reference_exists(false),
      m_non_embedded_reference_exists(false),
      m_single_pointer_reference_exists(false),
      m_array_pointer_reference_exists(false),
      m_array_reference_exists(false),
      m_reference_from_non_default_normal_block_exists(false),
      m_post_load_action(nullptr),
      m_block(nullptr)
{
}
