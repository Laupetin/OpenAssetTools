#pragma once
#include "Zone/Zone.h"

#include <string>
#include <vector>

class Zone;

class XAssetInfoGeneric
{
public:
    asset_type_t m_type = -1;
    std::string m_name;
    Zone* m_zone;
    std::vector<XAssetInfoGeneric*> m_dependencies;
    std::vector<scr_string_t> m_used_script_strings;
    void* m_ptr;
};

template<typename T> class XAssetInfo : public XAssetInfoGeneric
{
public:
    T* Asset()
    {
        return static_cast<T*>(m_ptr);
    }

    const T* Asset() const
    {
        return static_cast<const T*>(const_cast<const void*>(m_ptr));
    }
};
