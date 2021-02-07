#pragma once
#include "DeclarationModifier.h"

class PointerDeclarationModifier final : public DeclarationModifier
{
public:
    DeclarationModifierType GetType() override;
};
