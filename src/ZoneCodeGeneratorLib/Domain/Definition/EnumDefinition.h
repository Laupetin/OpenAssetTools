#pragma once

#include "BaseTypeDefinition.h"
#include "DataDefinition.h"
#include "EnumMember.h"

#include <memory>
#include <vector>

class EnumDefinition final : public DataDefinition
{
public:
    EnumDefinition(std::string _namespace, std::string name, const BaseTypeDefinition* parentType);

    [[nodiscard]] DataDefinitionType GetType() const override;
    [[nodiscard]] unsigned GetAlignment() const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize() const override;

    void AddEnumMember(EnumMember enumMember);

    const BaseTypeDefinition* m_parent_type;
    std::vector<std::unique_ptr<EnumMember>> m_members;
};
