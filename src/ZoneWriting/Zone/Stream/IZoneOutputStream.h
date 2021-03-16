#pragma once

#include <cstddef>
#include <typeindex>
#include <typeinfo>

#include "Zone/Stream/IZoneStream.h"

class IZoneOutputStream : public IZoneStream
{
public:
    static constexpr void* PTR_FOLLOWING = reinterpret_cast<void*>(-1);
    static constexpr void* PTR_INSERT = reinterpret_cast<void*>(-2);

    virtual void Align(int alignTo) = 0;

    virtual void* WriteDataRaw(const void* dst, size_t size) = 0;
    virtual void* WriteDataInBlock(const void* dst, size_t size) = 0;
    virtual void IncBlockPos(size_t size) = 0;
    virtual void WriteNullTerminated(const void* dst) = 0;

    virtual bool ReusableShouldWrite(void** pPtr, size_t size, size_t count, std::type_index type) = 0;
    virtual void MarkFollowing(void** pPtr) = 0;

    template<typename T>
    bool ReusableShouldWrite(T** pPtr)
    {
        return ReusableShouldWrite(reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(pPtr)), sizeof(T), 1, std::type_index(typeid(T)));
    }

    template<typename T>
    bool ReusableShouldWrite(T** pPtr, const size_t count)
    {
        return ReusableShouldWrite(reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(pPtr)), sizeof(T), count, std::type_index(typeid(T)));
    }

    template<typename T>
    T* Write(T* dst)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), sizeof(T)));
    }

    template<typename T>
    T* Write(T* dst, const uint32_t count)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), count * sizeof(T)));
    }

    template<typename T>
    T* WritePartial(T* dst, const size_t size)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), size));
    }

    template<typename T>
    void MarkFollowing(T*& ptr)
    {
        MarkFollowing(reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(&ptr)));
    }
};
