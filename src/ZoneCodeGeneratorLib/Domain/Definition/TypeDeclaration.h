#pragma once

#include <memory>
#include <vector>

#include "Utils/ClassUtils.h"
#include "DataDefinition.h"
#include "DeclarationModifier.h"

class TypeDeclaration
{
public:
    static constexpr int FLAG_FIELDS_CALCULATED = 1 << 0;
    static constexpr int FLAG_ALIGNMENT_FORCED = 1 << 1;

    unsigned m_flags;
    unsigned m_size;
    unsigned m_alignment;

    explicit TypeDeclaration(const DataDefinition* type);

    bool m_is_const;
    bool m_has_custom_bit_size;

    const DataDefinition* m_type;
    unsigned m_custom_bit_size;

    std::vector<std::unique_ptr<DeclarationModifier>> m_declaration_modifiers;

    _NODISCARD unsigned GetSize() const;
    _NODISCARD unsigned GetAlignment() const;
    _NODISCARD bool GetForceAlignment() const;
};
