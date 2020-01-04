#pragma once

#include "Zone/XBlock.h"
#include <vector>

class ZoneMemory
{
    class IDestructible
    {
    public:
        virtual ~IDestructible() = default;
    };

    template <class T>
    class Allocation final : public IDestructible
    {
    public:
        T m_entry;

        template <class... _Valty>
        explicit Allocation(_Valty&&... _Val)
            : m_entry(std::forward<_Valty>(_Val)...)
        {
        }

        ~Allocation() override = default;

        Allocation(const Allocation& other) = delete;
        Allocation(Allocation&& other) noexcept = delete;
        Allocation& operator=(const Allocation& other) = delete;
        Allocation& operator=(Allocation&& other) noexcept = delete;
    };

    class AllocationInfo
    {
    public:
        IDestructible* m_data;
        void* m_data_ptr;

        AllocationInfo(IDestructible* data, void* dataPtr);
    };

    std::vector<XBlock*> m_blocks;
    std::vector<void*> m_allocations;
    std::vector<AllocationInfo> m_destructible;

public:
    ZoneMemory();
    ~ZoneMemory();

    void AddBlock(XBlock* block);

    void* Alloc(size_t size);
    char* Dup(const char* str);

    template <class T, class... _Valty>
    T* Create(_Valty&&... _Val)
    {
        Allocation<T>* allocation = new Allocation<T>(std::forward<_Valty>(_Val)...);
        m_destructible.emplace_back(allocation, &allocation->m_entry);
        return &allocation->m_entry;
    }

    void Delete(void* data);
};
