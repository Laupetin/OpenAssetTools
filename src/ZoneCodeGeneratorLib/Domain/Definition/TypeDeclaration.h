#pragma once
#include <memory>
#include <vector>
#include <cstdint>

#include "DataDefinition.h"
#include "DeclarationModifier.h"

class TypeDeclaration
{
    static constexpr unsigned POINTER_SIZE = sizeof(uint32_t); // TODO: Change this to support 64bit
    static constexpr int FLAG_SIZE_CALCULATED = 1 << 0;
    static constexpr int FLAG_ALIGNMENT_CALCULATED = 1 << 1;
    static constexpr int FLAG_ALIGNMENT_FORCED = 1 << 2;

    unsigned m_flags;
    unsigned m_size;
    unsigned m_alignment;

    void CalculateSize();
    void CalculateAlignment();

public:
    explicit TypeDeclaration(const DataDefinition* type);

    bool m_is_const;
    bool m_has_custom_bit_size;

    const DataDefinition* m_type;
    unsigned m_custom_bit_size;

    std::vector<std::unique_ptr<DeclarationModifier>> m_declaration_modifiers;

    unsigned GetSize();
    unsigned GetAlignment();
    bool GetForceAlignment();
};
