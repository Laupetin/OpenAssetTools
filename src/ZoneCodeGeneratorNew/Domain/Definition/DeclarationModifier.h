#pragma once

enum class DeclarationModifierType
{
    POINTER,
    ARRAY
};

class DeclarationModifier
{
public:
    DeclarationModifier() = default;
    virtual ~DeclarationModifier() = default;

    virtual DeclarationModifierType GetType() = 0;
};