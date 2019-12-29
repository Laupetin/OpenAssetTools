#pragma once

#include "ObjContainer/IObjContainer.h"
#include "ObjContainer/ObjContainerReferenceable.h"
#include "Zone/Zone.h"

#include <string>
#include <vector>

class ObjContainerRegistry
{
    std::vector<IObjContainer*> m_containers;

public:
    void AddContainer(IObjContainer* container);
    void AddContainerWithReference(ObjContainerReferenceable* container, Zone* referencer);
    void RemoveContainerReferences(Zone* referencer);

    IObjContainer* GetContainerByName(const std::string& name);
};

extern ObjContainerRegistry g_ObjContainerRegistry;