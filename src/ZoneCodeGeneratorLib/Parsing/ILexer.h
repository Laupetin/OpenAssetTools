#pragma once

class ILexer
{
public:

    ILexer() = default;
    virtual ~ILexer() = default;
    ILexer(const ILexer& other) = default;
    ILexer(ILexer&& other) noexcept = default;
    ILexer& operator=(const ILexer& other) = default;
    ILexer& operator=(ILexer&& other) noexcept = default;
    
    virtual void PopTokens(int amount) = 0;
};
