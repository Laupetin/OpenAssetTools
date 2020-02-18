#pragma once
#include <vector>
#include <string>

class XAssetInfoGeneric
{
public:
    int m_type = -1;
    std::string m_name;
    std::vector<std::string> m_script_strings;
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
};
