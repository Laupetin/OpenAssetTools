#pragma once

#include "ObjContainer/IObjContainer.h"
#include "Utils/TransformIterator.h"

#include <algorithm>
#include <string>
#include <vector>
#include <set>

template <typename ContainerType, typename ReferencerType>
class ObjContainerRepository
{
    class ObjContainerEntry
    {
    public:
        ContainerType* m_container;
        std::set<ReferencerType*> m_references;

        explicit ObjContainerEntry(ContainerType* container)
        {
            m_container = container;
        }
    };

    std::vector<ObjContainerEntry> m_containers;

public:
    void AddContainer(ContainerType* container, ReferencerType* referencer)
    {
        auto firstEntry = std::find_if(m_containers.begin(), m_containers.end(),
                                       [container](ObjContainerEntry& entry) -> bool
                                       {
                                           return entry.m_container == container;
                                       });

        if(firstEntry != m_containers.end())
        {
            firstEntry->m_references.insert(referencer);
            return;
        }

        ObjContainerEntry entry(container);
        entry.m_references.insert(referencer);
        m_containers.push_back(entry);
    }

    void RemoveContainerReferences(ReferencerType* referencer)
    {
        for(auto iEntry = m_containers.begin(); iEntry != m_containers.end(); ++iEntry)
        {
            auto foundReference = iEntry->m_references.find(referencer);

            if(foundReference != iEntry->m_references.end())
            {
                iEntry->m_references.erase(foundReference);
            }

            if(iEntry->m_references.empty())
            {
                delete iEntry->m_container;
                m_containers.erase(iEntry);
            }
        }
    }

    IObjContainer* GetContainerByName(const std::string& name)
    {
        auto foundEntry = std::find_if(m_containers.begin(), m_containers.end(), [name](ObjContainerEntry& entry) -> bool
            {
                return entry.m_container->GetName() == name;
            });

        if(foundEntry != m_containers.end())
        {
            return foundEntry->m_container;
        }

        return nullptr;
    }

    TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*> begin()
    {
        return TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*>(
            m_containers.begin(), [](ObjContainerEntry& entry) -> ContainerType* { return entry.m_container; });
    }

    TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*> end()
    {
        return TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*>(
            m_containers.end(), [](ObjContainerEntry& entry) -> ContainerType* { return entry.m_container; });
    }
};
