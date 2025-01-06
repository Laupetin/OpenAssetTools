#pragma once

#include <memory>
#include <ostream>
#include <string>

class IOutputPath
{
public:
    IOutputPath() = default;
    virtual ~IOutputPath() = default;
    IOutputPath(const IOutputPath& other) = default;
    IOutputPath(IOutputPath&& other) noexcept = default;
    IOutputPath& operator=(const IOutputPath& other) = default;
    IOutputPath& operator=(IOutputPath&& other) noexcept = default;

    virtual std::unique_ptr<std::ostream> Open(const std::string& fileName) = 0;
};
