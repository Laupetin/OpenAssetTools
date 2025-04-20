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
    DeclarationModifier(const DeclarationModifier& other) = default;
    DeclarationModifier(DeclarationModifier&& other) noexcept = default;
    DeclarationModifier& operator=(const DeclarationModifier& other) = default;
    DeclarationModifier& operator=(DeclarationModifier&& other) noexcept = default;

    [[nodiscard]] virtual DeclarationModifierType GetType() const = 0;
};
