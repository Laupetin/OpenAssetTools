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
    static constexpr int FLAG_ANONYMOUS = 1 << 3;

    DefinitionWithMembers(std::string _namespace, std::string name, unsigned pack);

    [[nodiscard]] unsigned GetAlignment(WordSize wordSize) const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize(WordSize wordSize) const override;
    [[nodiscard]] bool IsAnonymous() const;

    unsigned m_flags;
    unsigned m_size[WORD_SIZE_COUNT];
    unsigned m_alignment[WORD_SIZE_COUNT];
    unsigned m_pack;

    std::vector<std::shared_ptr<Variable>> m_members;
};
