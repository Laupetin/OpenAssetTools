#include "Variable.h"

unsigned Variable::GetAlignment()
{
    if (m_has_alignment_override)
        return m_alignment_override;

    return m_type_declaration->GetAlignment();
}

bool Variable::GetForceAlignment()
{
    return m_has_alignment_override || m_type_declaration->GetForceAlignment();
}
