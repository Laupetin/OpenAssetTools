#pragma once

#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "TypeDeclaration.h"

class Variable
{
public:
    std::string m_name;
    bool m_has_alignment_override;
    unsigned m_alignment_override;
    unsigned m_offset;
    std::unique_ptr<TypeDeclaration> m_type_declaration;

    Variable(std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration);

    _NODISCARD unsigned GetAlignment() const;
    _NODISCARD bool GetForceAlignment() const;
};
