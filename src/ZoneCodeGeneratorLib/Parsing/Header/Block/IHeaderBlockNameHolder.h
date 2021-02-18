#pragma once

#include <string>

class IHeaderBlockNameHolder
{
public:
    IHeaderBlockNameHolder() = default;
    virtual ~IHeaderBlockNameHolder() = default;
    IHeaderBlockNameHolder(const IHeaderBlockNameHolder& other) = default;
    IHeaderBlockNameHolder(IHeaderBlockNameHolder&& other) noexcept = default;
    IHeaderBlockNameHolder& operator=(const IHeaderBlockNameHolder& other) = default;
    IHeaderBlockNameHolder& operator=(IHeaderBlockNameHolder&& other) noexcept = default;

    virtual void SetBlockName(const TokenPos& nameTokenPos, std::string name) = 0;
};
