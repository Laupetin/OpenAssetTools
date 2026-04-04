#pragma once

#include "Utils/ProgressCallback.h"
#include "Zone/Zone.h"

#include <expected>
#include <string>

class ZoneLoading
{
public:
    static std::expected<std::unique_ptr<Zone>, std::string> LoadZone(const std::string& path,
                                                                      std::optional<std::unique_ptr<ProgressCallback>> progressCallback);
};
