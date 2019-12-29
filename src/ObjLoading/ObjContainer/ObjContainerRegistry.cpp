#include "ObjContainerRegistry.h"
#include <cassert>

ObjContainerRegistry g_ObjContainerRegistry;

IObjContainer* ObjContainerRegistry::GetContainerByName(const std::string& name)
{
    for (auto* container : m_containers)
    {
        if (container->GetName() == name)
        {
            return container;
        }
    }

    return nullptr;
}

void ObjContainerRegistry::AddContainer(IObjContainer* container)
{
    assert(dynamic_cast<ObjContainerReferenceable*>(container) == nullptr);

    m_containers.push_back(container);
}

void ObjContainerRegistry::AddContainerWithReference(ObjContainerReferenceable* container, Zone* referencer)
{
    container->AddReference(referencer);
    m_containers.push_back(container);
}

void ObjContainerRegistry::RemoveContainerReferences(Zone* referencer)
{
    auto iContainer = m_containers.begin();

    while (iContainer != m_containers.end())
    {
        auto* container = *iContainer;

        if (auto* referenceableContainer = dynamic_cast<ObjContainerReferenceable*>(container))
        {
            if (referenceableContainer->RemoveReference(referencer) && !referenceableContainer->IsReferenced())
            {
                delete container;
                iContainer = m_containers.erase(iContainer);
                continue;
            }
        }

        ++iContainer;
    }
}
