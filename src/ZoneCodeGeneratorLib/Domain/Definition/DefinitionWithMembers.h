#pragma once

#include "DataDefinition.h"
#include "Variable.h"

#include <memory>
#include <vector>

class DefinitionWithMembers : public DataDefinition
{
public:
    static constexpr int FLAG_FIELDS_CALCULATED = 1 << 0;
    static constexpr int FLAG_FIELDS_CALCULATING = 1 << 1;
    static constexpr int FLAG_ALIGNMENT_FORCED = 1 << 2;

    DefinitionWithMembers(std::string _namespace, std::string name, unsigned pack);

    [[nodiscard]] unsigned GetAlignment() const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize() const override;

    unsigned m_flags;
    unsigned m_size;
    unsigned m_alignment;

    bool m_has_alignment_override;
    bool m_anonymous;

    const unsigned m_pack;
    unsigned m_alignment_override;

    std::vector<std::shared_ptr<Variable>> m_members;
};
