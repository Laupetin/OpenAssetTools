#pragma once

class ZoneCodeGenerator
{
    class Impl;
    Impl* m_impl;

public:
    ZoneCodeGenerator();
    ~ZoneCodeGenerator();
    ZoneCodeGenerator(const ZoneCodeGenerator& other) = delete;
    ZoneCodeGenerator(ZoneCodeGenerator&& other) noexcept = default;
    ZoneCodeGenerator& operator=(const ZoneCodeGenerator& other) = delete;
    ZoneCodeGenerator& operator=(ZoneCodeGenerator&& other) noexcept = default;

    int Run(int argc, const char** argv) const;
};
