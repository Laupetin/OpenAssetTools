#pragma once
#include <vector>
#include "Zone/XAssetDependency.h"

template<typename T>
class XAssetInfo
{
public:
    std::string m_name;
    T* m_asset;
    std::vector<std::string> m_script_strings;
    std::vector<XAssetDependency> m_dependencies;
};
