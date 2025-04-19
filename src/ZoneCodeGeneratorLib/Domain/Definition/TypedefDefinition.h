#pragma once

#include "DataDefinition.h"
#include "TypeDeclaration.h"

class TypedefDefinition final : public DataDefinition
{
public:
    TypedefDefinition(std::string _namespace, std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration);

    [[nodiscard]] DataDefinitionType GetType() const override;
    [[nodiscard]] unsigned GetAlignment() const override;
    [[nodiscard]] bool GetForceAlignment() const override;
    [[nodiscard]] unsigned GetSize() const override;

    bool m_has_alignment_override;
    unsigned m_alignment_override;
    std::unique_ptr<TypeDeclaration> m_type_declaration;
};
