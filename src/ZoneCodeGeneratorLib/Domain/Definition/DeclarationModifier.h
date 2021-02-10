#pragma once

#include "Utils/ClassUtils.h"

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

    _NODISCARD virtual DeclarationModifierType GetType() const = 0;
};