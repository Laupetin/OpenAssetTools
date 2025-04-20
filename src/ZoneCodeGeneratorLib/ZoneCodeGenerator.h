#pragma once

#include <memory>

class ZoneCodeGenerator
{
protected:
    ZoneCodeGenerator() = default;

public:
    virtual ~ZoneCodeGenerator() = default;
    virtual int Run(int argc, const char** argv) = 0;

    static std::unique_ptr<ZoneCodeGenerator> Create();
};
