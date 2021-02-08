#pragma once

#include "DataDefinition.h"

class ForwardDeclaration final : public DataDefinition
{
public:
    const DataDefinitionType m_forwarded_type;
    DataDefinition* m_definition;

    ForwardDeclaration(std::string _namespace, std::string name, DataDefinitionType type);

    _NODISCARD DataDefinitionType GetType() const override;
    _NODISCARD unsigned GetAlignment() override;
    _NODISCARD bool GetForceAlignment() override;
    _NODISCARD unsigned GetSize() override;
};
