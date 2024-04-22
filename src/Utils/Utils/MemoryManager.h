#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

class MemoryManager
{
    class IDestructible
    {
    public:
        IDestructible() = default;
        virtual ~IDestructible() = default;
        IDestructible(const IDestructible& other) = default;
        IDestructible(IDestructible&& other) noexcept = default;
        IDestructible& operator=(const IDestructible& other) = default;
        IDestructible& operator=(IDestructible&& other) noexcept = default;
    };

    template<class T> class Allocation final : public IDestructible
    {
    public:
        T m_entry;

        template<class... ValType>
        explicit Allocation(ValType&&... val)
            : m_entry(std::forward<ValType>(val)...)
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

    std::vector<void*> m_allocations;
    std::vector<AllocationInfo> m_destructible;

public:
    MemoryManager();
    virtual ~MemoryManager();
    MemoryManager(const MemoryManager& other) = delete;
    MemoryManager(MemoryManager&& other) noexcept = default;
    MemoryManager& operator=(const MemoryManager& other) = delete;
    MemoryManager& operator=(MemoryManager&& other) noexcept = default;

    void* AllocRaw(size_t size);
    char* Dup(const char* str);

    template<typename T> std::add_pointer_t<T> Alloc(const size_t count = 1u)
    {
        return static_cast<std::add_pointer_t<T>>(AllocRaw(sizeof(T) * count));
    }

    template<class T, class... ValType> std::add_pointer_t<T> Create(ValType&&... val)
    {
        Allocation<T>* allocation = new Allocation<T>(std::forward<ValType>(val)...);
        m_destructible.emplace_back(allocation, &allocation->m_entry);
        return &allocation->m_entry;
    }

    void Free(const void* data);
    void Delete(const void* data);
};
