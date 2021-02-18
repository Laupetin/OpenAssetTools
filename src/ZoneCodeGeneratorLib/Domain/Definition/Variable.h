#pragma once

#include <memory>
#include <string>

#include "TypeDeclaration.h"

class Variable
{
public:
    std::string m_name;
    bool m_has_alignment_override;
    unsigned m_alignment_override;
    std::unique_ptr<TypeDeclaration> m_type_declaration;

    Variable(std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration);

    unsigned GetAlignment();
    bool GetForceAlignment();
};
