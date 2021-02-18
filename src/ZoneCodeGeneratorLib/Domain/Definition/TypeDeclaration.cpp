#include "TypeDeclaration.h"

#include <cassert>

#include "ArrayDeclarationModifier.h"

TypeDeclaration::TypeDeclaration(const DataDefinition* type)
    : m_flags(0),
      m_size(0),
      m_alignment(0),
      m_is_const(false),
      m_has_custom_bit_size(false),
      m_type(type),
      m_custom_bit_size(0)
{
    assert(m_type != nullptr);
}

void TypeDeclaration::CalculateSize()
{
    auto currentSize = m_type->GetSize();

    for (auto i = m_declaration_modifiers.size(); i > 0; i--)
    {
        const auto& declarationModifier = m_declaration_modifiers[i - 1];

        switch (declarationModifier->GetType())
        {
        case DeclarationModifierType::POINTER:
            currentSize = POINTER_SIZE;
            break;

        case DeclarationModifierType::ARRAY:
            currentSize *= dynamic_cast<ArrayDeclarationModifier*>(declarationModifier.get())->m_size;
            break;
        }
    }

    m_flags |= FLAG_SIZE_CALCULATED;
}

void TypeDeclaration::CalculateAlignment()
{
    auto hasPointerModifier = false;
    for (const auto& declarationModifier : m_declaration_modifiers)
    {
        if (declarationModifier->GetType() == DeclarationModifierType::POINTER)
        {
            hasPointerModifier = true;
            break;
        }
    }

    if (hasPointerModifier)
    {
        m_alignment = POINTER_SIZE;
    }
    else
    {
        m_alignment = m_type->GetAlignment();
        if (m_type->GetForceAlignment())
            m_flags |= FLAG_ALIGNMENT_FORCED;
    }

    m_flags |= FLAG_ALIGNMENT_CALCULATED;
}

unsigned TypeDeclaration::GetSize()
{
    if ((m_flags & FLAG_SIZE_CALCULATED) == 0)
        CalculateSize();

    return m_size;
}

unsigned TypeDeclaration::GetAlignment()
{
    if ((m_flags & FLAG_ALIGNMENT_CALCULATED) == 0)
        CalculateAlignment();

    return m_alignment;
}

bool TypeDeclaration::GetForceAlignment()
{
    if ((m_flags & FLAG_ALIGNMENT_CALCULATED) == 0)
        CalculateAlignment();

    return m_flags & FLAG_ALIGNMENT_FORCED;
}
