#pragma once
#include <vector>
#include <string>

#include "Zone/Zone.h"

class Zone;

class XAssetInfoGeneric
{
public:
    int m_type = -1;
    std::string m_name;
    Zone* m_zone;
    std::vector<XAssetInfoGeneric*> m_dependencies;
    void* m_ptr;
};

template<typename T>
class XAssetInfo : public XAssetInfoGeneric
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
