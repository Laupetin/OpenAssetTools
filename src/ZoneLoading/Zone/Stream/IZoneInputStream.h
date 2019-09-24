#pragma once

#include "Zone/Stream/IZoneStream.h"
#include <cstdint>

class IZoneInputStream : public IZoneStream
{
public:
    virtual void* Alloc(int align) = 0;

    template<typename T>
    T* Alloc(const int align)
    {
        return static_cast<T*>(Alloc(align));
    }

    template<typename T>
    T* Alloc()
    {
        return static_cast<T*>(Alloc(alignof(T)));
    }

    virtual void LoadData(size_t size) = 0;
    virtual void LoadNullTerminated() = 0;

    template<typename T>
    void Load()
    {
        LoadData(sizeof(T));
    }

    template<typename T>
    void Load(const uint32_t count)
    {
        LoadData(count * sizeof(T));
    }

    virtual void** InsertPointer() = 0;
    template<typename T>
    T** InsertPointer()
    {
        return reinterpret_cast<T**>(InsertPointer());
    }

    virtual void* ConvertOffsetToPointer(const void* offset) = 0;
    template<typename T>
    T* ConvertOffsetToPointer(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToPointer(static_cast<const void*>(offset)));
    }

    virtual void* ConvertOffsetToAlias(const void* offset) = 0;
    template<typename T>
    T* ConvertOffsetToAlias(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToAlias(static_cast<const void*>(offset)));
    }
};
