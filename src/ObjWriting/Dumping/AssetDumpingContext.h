#pragma once

#include <string>

#include "Zone/Zone.h"

class AssetDumpingContext
{
public:
    Zone* m_zone;
    std::string m_base_path;
};
