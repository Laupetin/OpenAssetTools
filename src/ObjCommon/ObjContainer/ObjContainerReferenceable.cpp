#include "ObjContainerReferenceable.h"

void ObjContainerReferenceable::AddReference(Zone* referencer)
{
    m_references.insert(referencer);
}

bool ObjContainerReferenceable::RemoveReference(Zone* zone)
{
    return m_references.erase(zone) > 0;
}

bool ObjContainerReferenceable::IsReferenced() const
{
    return !m_references.empty();
}
