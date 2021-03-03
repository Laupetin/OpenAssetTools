#include "ObjContainerReferenceable.h"

void ObjContainerReferenceable::AddReference(void* referencer)
{
    m_references.insert(referencer);
}

bool ObjContainerReferenceable::RemoveReference(void* referencer)
{
    return m_references.erase(referencer) > 0;
}

bool ObjContainerReferenceable::IsReferenced() const
{
    return !m_references.empty();
}
