#pragma once

#include "DataDefinition.h"
#include "DeclarationModifier.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <vector>

class TypeDeclaration
{
public:
    static constexpr int FLAG_FIELDS_CALCULATED = 1 << 0;
    static constexpr int FLAG_ALIGNMENT_FORCED = 1 << 1;

    explicit TypeDeclaration(const DataDefinition* type);

    [[nodiscard]] unsigned GetSize() const;
    [[nodiscard]] unsigned GetAlignment() const;
    [[nodiscard]] bool GetForceAlignment() const;

    std::vector<std::unique_ptr<DeclarationModifier>> m_declaration_modifiers;

    bool m_is_const;
    bool m_has_custom_bit_size;

    const DataDefinition* m_type;
    unsigned m_custom_bit_size;

    unsigned m_flags;
    unsigned m_size;
    unsigned m_alignment;
};
