#include "XAssetInfo.h"

#include "Utils/StringUtils.h"

#include <algorithm>

IndirectAssetReference::IndirectAssetReference()
    : m_type(-1)
{
}

IndirectAssetReference::IndirectAssetReference(const asset_type_t type, std::string name)
    : m_type(type),
      m_name(std::move(name))
{
    utils::MakeStringLowerCase(m_name);
}

bool operator==(const IndirectAssetReference& lhs, const IndirectAssetReference& rhs)
{
    return lhs.m_type == rhs.m_type && lhs.m_name == rhs.m_name;
}

bool operator!=(const IndirectAssetReference& lhs, const IndirectAssetReference& rhs)
{
    return !(lhs == rhs);
}

std::size_t std::hash<IndirectAssetReference>::operator()(const IndirectAssetReference& v) const noexcept
{
    std::size_t seed = 0x01A627D3;
    seed ^= (seed << 6) + (seed >> 2) + 0x0BCEF9CE + static_cast<std::size_t>(v.m_type);
    seed ^= (seed << 6) + (seed >> 2) + 0x7225148B + std::hash<std::string>()(v.m_name);
    return seed;
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

XAssetInfoGeneric::XAssetInfoGeneric(asset_type_t type,
                                     std::string name,
                                     void* ptr,
                                     std::vector<XAssetInfoGeneric*> dependencies,
                                     std::vector<scr_string_t> usedScriptStrings,
                                     std::vector<IndirectAssetReference> indirectAssetReferences)
    : m_type(type),
      m_name(std::move(name)),
      m_ptr(ptr),
      m_dependencies(std::move(dependencies)),
      m_used_script_strings(std::move(usedScriptStrings)),
      m_indirect_asset_references(std::move(indirectAssetReferences)),
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

bool XAssetInfoGeneric::IsReference() const
{
    return !m_name.empty() && m_name[0] == ',';
}

std::string XAssetInfoGeneric::ReferencedAssetName() const
{
    return m_name.substr(1);
}

std::string XAssetInfoGeneric::NormalizeAssetName(std::string input)
{
    utils::MakeStringLowerCase(input);

    std::ranges::replace(input, '\\', '/');

    return input;
}
