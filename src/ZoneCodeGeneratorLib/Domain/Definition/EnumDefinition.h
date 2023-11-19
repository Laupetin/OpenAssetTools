#pragma once

#include "BaseTypeDefinition.h"
#include "DataDefinition.h"
#include "EnumMember.h"

#include <memory>
#include <vector>

class EnumDefinition final : public DataDefinition
{
public:
    const BaseTypeDefinition* m_parent_type;
    std::vector<std::unique_ptr<EnumMember>> m_members;

    EnumDefinition(std::string _namespace, std::string name, const BaseTypeDefinition* parentType);

    _NODISCARD DataDefinitionType GetType() const override;
    _NODISCARD unsigned GetAlignment() const override;
    _NODISCARD bool GetForceAlignment() const override;
    _NODISCARD unsigned GetSize() const override;

    void AddEnumMember(EnumMember enumMember);
};
