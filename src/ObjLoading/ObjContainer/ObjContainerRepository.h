#pragma once

#include "ObjContainer/IObjContainer.h"
#include "Utils/TransformIterator.h"

#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <memory>

template <typename ContainerType, typename ReferencerType>
class ObjContainerRepository
{
    class ObjContainerEntry
    {
    public:
        std::unique_ptr<ContainerType> m_container;
        std::set<ReferencerType*> m_references;

        explicit ObjContainerEntry(std::unique_ptr<ContainerType> container)
            : m_container(std::move(container))
        {
        }

        ~ObjContainerEntry() = default;
        ObjContainerEntry(const ObjContainerEntry& other) = delete;
        ObjContainerEntry(ObjContainerEntry&& other) noexcept = default;
        ObjContainerEntry& operator=(const ObjContainerEntry& other) = delete;
        ObjContainerEntry& operator=(ObjContainerEntry&& other) noexcept = default;
    };

    std::vector<ObjContainerEntry> m_containers;

public:
    ObjContainerRepository() = default;
    ~ObjContainerRepository() = default;
    ObjContainerRepository(const ObjContainerRepository& other) = delete;
    ObjContainerRepository(ObjContainerRepository&& other) noexcept = default;
    ObjContainerRepository& operator=(const ObjContainerRepository& other) = delete;
    ObjContainerRepository& operator=(ObjContainerRepository&& other) noexcept = default;

    void AddContainer(std::unique_ptr<ContainerType> container, ReferencerType* referencer)
    {
        ObjContainerEntry entry(std::move(container));
        entry.m_references.insert(referencer);
        m_containers.emplace_back(std::move(entry));
    }

    bool AddContainerReference(ContainerType* container, ReferencerType* referencer)
    {
        auto firstEntry = std::find_if(m_containers.begin(), m_containers.end(), [container](const ObjContainerEntry& entry)
        {
            return entry.m_container.get() == container;
        });

        if (firstEntry != m_containers.end())
        {
            firstEntry->m_references.insert(referencer);
            return true;
        }

        return false;
    }

    void RemoveContainerReferences(ReferencerType* referencer)
    {
        for (auto iEntry = m_containers.begin(); iEntry != m_containers.end();)
        {
            auto foundReference = iEntry->m_references.find(referencer);

            if (foundReference != iEntry->m_references.end())
            {
                iEntry->m_references.erase(foundReference);
            }

            if (iEntry->m_references.empty())
            {
                iEntry = m_containers.erase(iEntry);
            }
            else
            {
                ++iEntry;
            }
        }
    }

    ContainerType* GetContainerByName(const std::string& name)
    {
        auto foundEntry = std::find_if(m_containers.begin(), m_containers.end(), [name](ObjContainerEntry& entry)
        {
            return entry.m_container->GetName() == name;
        });

        if (foundEntry != m_containers.end())
        {
            return foundEntry->m_container.get();
        }

        return nullptr;
    }

    TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*> begin()
    {
        return TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*>(
            m_containers.begin(), [](ObjContainerEntry& entry)  { return entry.m_container.get(); });
    }

    TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*> end()
    {
        return TransformIterator<typename std::vector<ObjContainerEntry>::iterator, ObjContainerEntry&, ContainerType*>(
            m_containers.end(), [](ObjContainerEntry& entry){ return entry.m_container.get(); });
    }
};
