#pragma once

#include <cstddef>
#include <typeindex>
#include <typeinfo>

#include "Zone/Stream/IZoneStream.h"

class IZoneOutputStream : public IZoneStream
{
public:
    virtual void Align(int alignTo) = 0;

    virtual void* WriteDataRaw(void* dst, size_t size) = 0;
    virtual void* WriteDataInBlock(void* dst, size_t size) = 0;
    virtual void IncBlockPos(size_t size) = 0;
    virtual void WriteNullTerminated(void* dst) = 0;

    virtual bool ReusableShouldWrite(void** pPtr, size_t size, size_t count, std::type_index type) = 0;

    template<typename T>
    bool ReusableShouldWrite(T** pPtr)
    {
        return ReusableShouldWrite(pPtr, sizeof(T), 1, std::type_index(typeid(T)));
    }

    template<typename T>
    bool ReusableShouldWrite(T** pPtr, size_t count)
    {
        return ReusableShouldWrite(pPtr, sizeof(T), count, std::type_index(typeid(T)));
    }

    template<typename T>
    T* Write(T* dst)
    {
        return static_cast<T*>(WriteDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), sizeof(T)));
    }

    template<typename T>
    T* Write(T* dst, const uint32_t count)
    {
        return static_cast<T*>(WriteDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), count * sizeof(T)));
    }

    template<typename T>
    T* WritePartial(T* dst, const size_t size)
    {
        return static_cast<T*>(WriteDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), size));
    }
};
