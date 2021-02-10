#pragma once

#include "DataDefinition.h"
#include "TypeDeclaration.h"

class TypedefDefinition final : public DataDefinition
{
public:
    bool m_has_alignment_override;
    unsigned m_alignment_override;
    std::unique_ptr<TypeDeclaration> m_type_declaration;

    TypedefDefinition(std::string _namespace, std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration);

    _NODISCARD DataDefinitionType GetType() const override;
    _NODISCARD unsigned GetAlignment() override;
    _NODISCARD bool GetForceAlignment() override;
    _NODISCARD unsigned GetSize() override;
};
