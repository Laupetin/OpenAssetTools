#pragma once

#include "Utils/ClassUtils.h"
#include "TokenPos.h"

class IParserValue
{
protected:
    IParserValue() = default;

public:
    virtual ~IParserValue() = default;
    IParserValue(const IParserValue& other) = default;
    IParserValue(IParserValue&& other) noexcept = default;
    IParserValue& operator=(const IParserValue& other) = default;
    IParserValue& operator=(IParserValue&& other) noexcept = default;

    _NODISCARD virtual bool IsEof() const = 0;
    _NODISCARD virtual const TokenPos& GetPos() const = 0;
};
