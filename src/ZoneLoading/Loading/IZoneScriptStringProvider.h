#pragma once

#include "Zone/ZoneTypes.h"
#include <string>

class IZoneScriptStringProvider
{
public:
    virtual std::string& GetZoneScriptString(scr_string_t scrString) = 0;
};
