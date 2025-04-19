#pragma once

#include "TypeDeclaration.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>

class Variable
{
public:
    Variable(std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration);

    [[nodiscard]] unsigned GetAlignment() const;
    [[nodiscard]] bool GetForceAlignment() const;

    std::string m_name;
    bool m_has_alignment_override;
    unsigned m_alignment_override;
    unsigned m_offset;
    std::unique_ptr<TypeDeclaration> m_type_declaration;
};
