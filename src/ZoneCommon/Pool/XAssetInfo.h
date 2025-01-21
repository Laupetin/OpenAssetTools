#pragma once
#include "Zone/Zone.h"
#include "Zone/ZoneTypes.h"

#include <string>
#include <vector>

class Zone;

// An indirect asset reference is a reference that is not done per pointer but by mentioning the asset
// in a string or requiring the asset to exist in some other way
class IndirectAssetReference
{
public:
    asset_type_t m_type;
    std::string m_name;

    IndirectAssetReference();
    IndirectAssetReference(asset_type_t type, std::string name);

    friend bool operator==(const IndirectAssetReference& lhs, const IndirectAssetReference& rhs);
    friend bool operator!=(const IndirectAssetReference& lhs, const IndirectAssetReference& rhs);
};

template<> struct std::hash<IndirectAssetReference>
{
    std::size_t operator()(const IndirectAssetReference& v) const noexcept;
};

class XAssetInfoGeneric
{
public:
    XAssetInfoGeneric();
    XAssetInfoGeneric(asset_type_t type, std::string name, void* ptr);
    XAssetInfoGeneric(
        asset_type_t type, std::string name, void* ptr, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings);
    XAssetInfoGeneric(asset_type_t type,
                      std::string name,
                      void* ptr,
                      std::vector<XAssetInfoGeneric*> dependencies,
                      std::vector<scr_string_t> usedScriptStrings,
                      std::vector<IndirectAssetReference> indirectAssetReferences);
    XAssetInfoGeneric(asset_type_t type,
                      std::string name,
                      void* ptr,
                      std::vector<XAssetInfoGeneric*> dependencies,
                      std::vector<scr_string_t> usedScriptStrings,
                      std::vector<IndirectAssetReference> indirectAssetReferences,
                      Zone* zone);
    ~XAssetInfoGeneric() = default;

    XAssetInfoGeneric(const XAssetInfoGeneric& other) = default;
    XAssetInfoGeneric(XAssetInfoGeneric&& other) noexcept = default;
    XAssetInfoGeneric& operator=(const XAssetInfoGeneric& other) = default;
    XAssetInfoGeneric& operator=(XAssetInfoGeneric&& other) noexcept = default;

    [[nodiscard]] bool IsReference() const;

    static std::string NormalizeAssetName(std::string input);

    asset_type_t m_type;
    std::string m_name;
    void* m_ptr;
    std::vector<XAssetInfoGeneric*> m_dependencies;
    std::vector<scr_string_t> m_used_script_strings;
    std::vector<IndirectAssetReference> m_indirect_asset_references;
    Zone* m_zone;
};

template<typename T> class XAssetInfo : public XAssetInfoGeneric
{
public:
    XAssetInfo() = default;

    XAssetInfo(const asset_type_t type, std::string name, T* ptr)
        : XAssetInfoGeneric(type, std::move(name), static_cast<void*>(ptr))
    {
    }

    XAssetInfo(const asset_type_t type, std::string name, T* ptr, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings)
        : XAssetInfoGeneric(type, std::move(name), static_cast<void*>(ptr), std::move(dependencies), std::move(usedScriptStrings))
    {
    }

    XAssetInfo(const asset_type_t type,
               std::string name,
               T* ptr,
               std::vector<XAssetInfoGeneric*> dependencies,
               std::vector<scr_string_t> usedScriptStrings,
               std::vector<IndirectAssetReference> indirectAssetReferences)
        : XAssetInfoGeneric(
              type, std::move(name), static_cast<void*>(ptr), std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences))
    {
    }

    XAssetInfo(const asset_type_t type,
               std::string name,
               T* ptr,
               std::vector<XAssetInfoGeneric*> dependencies,
               std::vector<scr_string_t> usedScriptStrings,
               std::vector<IndirectAssetReference> indirectAssetReferences,
               Zone* zone)
        : XAssetInfoGeneric(
              type, std::move(name), static_cast<void*>(ptr), std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences), zone)
    {
    }

    ~XAssetInfo() = default;

    XAssetInfo(const XAssetInfo& other) = default;
    XAssetInfo(XAssetInfo&& other) noexcept = default;
    XAssetInfo& operator=(const XAssetInfo& other) = default;
    XAssetInfo& operator=(XAssetInfo&& other) noexcept = default;

    T* Asset()
    {
        return static_cast<T*>(m_ptr);
    }

    const T* Asset() const
    {
        return static_cast<const T*>(const_cast<const void*>(m_ptr));
    }
};
