#pragma once

#include "Utils/FileAPI.h"
#include <string>

class ISearchPath
{
public:
    virtual ~ISearchPath() = default;

    virtual FileAPI::IFile* Open(const std::string& fileName) = 0;
};
