#pragma once

#include <string>

namespace menu
{
    class IWriter
    {
    public:
        IWriter() = default;
        virtual ~IWriter() = default;

        virtual void Start() = 0;
        virtual void End() = 0;

        virtual void IncludeMenu(const std::string& menuPath) const = 0;
    };
} // namespace menu
