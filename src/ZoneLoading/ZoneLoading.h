#pragma once

#include "Utils/Result.h"
#include "Zone/Zone.h"

#include <string>

class ZoneLoading
{
public:
    static result::Expected<std::unique_ptr<Zone>, std::string> LoadZone(const std::string& path);
};
