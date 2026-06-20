#pragma once

#include "SearchPath/ISearchPath.h"

#include <memory>
#include <string>

namespace iwd
{
    std::unique_ptr<ISearchPath> LoadFromFile(const std::string& path);
}
