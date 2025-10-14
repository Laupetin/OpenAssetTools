#pragma once

#include "Utils/ProgressCallback.h"
#include "Utils/Result.h"
#include "Zone/Zone.h"

#include <string>

class ZoneLoading
{
public:
    static result::Expected<std::unique_ptr<Zone>, std::string> LoadZone(const std::string& path,
                                                                         std::optional<std::unique_ptr<ProgressCallback>> progressCallback);
};
