#pragma once

#include "DataDefinition.h"

class ForwardDeclaration final : public DataDefinition
{
public:
    const DataDefinitionType m_forwarded_type;
    const DataDefinition* m_definition;

    ForwardDeclaration(std::string _namespace, std::string name, DataDefinitionType type);

    _NODISCARD DataDefinitionType GetType() const override;
    _NODISCARD unsigned GetAlignment() const override;
    _NODISCARD bool GetForceAlignment() const override;
    _NODISCARD unsigned GetSize() const override;
};
