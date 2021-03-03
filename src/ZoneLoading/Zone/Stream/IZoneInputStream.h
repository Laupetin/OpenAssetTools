#pragma once

#include <cstdint>
#include <cstddef>

#include "Zone/Stream/IZoneStream.h"

class IZoneInputStream : public IZoneStream
{
public:
    virtual void* Alloc(int align) = 0;

    template<typename T>
    T* Alloc(const int align)
    {
        return static_cast<T*>(Alloc(align));
    }

    virtual void LoadDataRaw(void* dst, size_t size) = 0;
    virtual void LoadDataInBlock(void* dst, size_t size) = 0;
    virtual void IncBlockPos(size_t size) = 0;
    virtual void LoadNullTerminated(void* dst) = 0;

    template<typename T>
    void Load(T* dst)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), sizeof(T));
    }

    template<typename T>
    void Load(T* dst, const uint32_t count)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), count * sizeof(T));
    }

    template<typename T>
    void LoadPartial(T* dst, const size_t size)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), size);
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
