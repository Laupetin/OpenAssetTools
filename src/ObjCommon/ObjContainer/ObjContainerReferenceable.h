#pragma once

#include "IObjContainer.h"

#include <set>

class ObjContainerReferenceable : public IObjContainer
{
    std::set<void*> m_references;

public:
    void AddReference(void* referencer);
    bool RemoveReference(void* referencer);
    bool IsReferenced() const;
};
