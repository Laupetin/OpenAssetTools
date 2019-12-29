#pragma once

#include "IObjContainer.h"
#include "Zone/Zone.h"

#include <set>

class ObjContainerReferenceable : public IObjContainer
{
    std::set<Zone*> m_references;

public:
    void AddReference(Zone* referencer);
    bool RemoveReference(Zone* zone);
    bool IsReferenced() const;
};
