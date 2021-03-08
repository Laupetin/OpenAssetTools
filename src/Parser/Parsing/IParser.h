#pragma once

class IParser
{
public:
    IParser() = default;
    virtual ~IParser() = default;
    IParser(const IParser& other) = default;
    IParser(IParser&& other) noexcept = default;
    IParser& operator=(const IParser& other) = default;
    IParser& operator=(IParser&& other) noexcept = default;

    virtual bool Parse() = 0;
};
