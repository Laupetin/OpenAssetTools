#pragma once

#include "DataDefinition.h"

class ForwardDeclaration final : public DataDefinition
{
public:
    ForwardDeclaration(std::string _namespace, std::string name, DataDefinitionType type);

    [[nodiscard]] DataDefinitionType GetType() const override;
    [[nodiscard]] unsigned GetAlignment() const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize() const override;

    const DataDefinitionType m_forwarded_type;
    const DataDefinition* m_definition;
};
