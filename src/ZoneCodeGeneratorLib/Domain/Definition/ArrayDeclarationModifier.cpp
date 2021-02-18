#include "ArrayDeclarationModifier.h"

ArrayDeclarationModifier::ArrayDeclarationModifier(const int size)
    : m_size(size)
{
}

DeclarationModifierType ArrayDeclarationModifier::GetType() const
{
    return DeclarationModifierType::ARRAY;
}
