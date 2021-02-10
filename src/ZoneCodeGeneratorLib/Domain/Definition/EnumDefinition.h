#pragma once

#include <memory>
#include <vector>

#include "BaseTypeDefinition.h"
#include "DataDefinition.h"
#include "EnumMember.h"

class EnumDefinition final : public DataDefinition
{
public:
    const BaseTypeDefinition* m_parent_type;
    std::vector<std::unique_ptr<EnumMember>> m_members;

    EnumDefinition(std::string _namespace, std::string name, const BaseTypeDefinition* parentType);

    _NODISCARD DataDefinitionType GetType() const override;
    _NODISCARD unsigned GetAlignment() override;
    _NODISCARD bool GetForceAlignment() override;
    _NODISCARD unsigned GetSize() override;

    void AddEnumMember(EnumMember enumMember);
};
