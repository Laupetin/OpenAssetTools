#include "XAssetInfo.h"

IndirectAssetReference::IndirectAssetReference()
    : m_type(-1)
{
}

IndirectAssetReference::IndirectAssetReference(const asset_type_t type, std::string name)
    : m_type(type),
      m_name(std::move(name))
{
}

XAssetInfoGeneric::XAssetInfoGeneric()
    : m_type(-1),
      m_ptr(nullptr),
      m_zone(nullptr)
{
}

XAssetInfoGeneric::XAssetInfoGeneric(const asset_type_t type, std::string name, void* ptr)
    : m_type(type),
      m_name(std::move(name)),
      m_ptr(ptr),
      m_zone(nullptr)
{
}

XAssetInfoGeneric::XAssetInfoGeneric(
    const asset_type_t type, std::string name, void* ptr, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings)
    : m_type(type),
      m_name(std::move(name)),
      m_ptr(ptr),
      m_dependencies(std::move(dependencies)),
      m_used_script_strings(std::move(usedScriptStrings)),
      m_zone(nullptr)
{
}

XAssetInfoGeneric::XAssetInfoGeneric(const asset_type_t type,
                                     std::string name,
                                     void* ptr,
                                     std::vector<XAssetInfoGeneric*> dependencies,
                                     std::vector<scr_string_t> usedScriptStrings,
                                     std::vector<IndirectAssetReference> indirectAssetReferences,
                                     Zone* zone)
    : m_type(type),
      m_name(std::move(name)),
      m_ptr(ptr),
      m_dependencies(std::move(dependencies)),
      m_used_script_strings(std::move(usedScriptStrings)),
      m_indirect_asset_references(std::move(indirectAssetReferences)),
      m_zone(zone)
{
}
