#pragma once

#include "InMemoryZoneData.h"
#include "Zone/Stream/IZoneStream.h"
#include "Zone/XBlock.h"

#include <cstddef>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <vector>

class ZoneOutputStream : public IZoneStream
{
public:
    /**
     * \brief Returns the configured bits that make up a pointer.
     */
    [[nodiscard]] virtual unsigned GetPointerBitCount() const = 0;

    /**
     * \brief Aligns the write position in the current block with the specified value.
     * \param align The alignment value that the write position is aligned with. This should typically be the alignment of the struct that
     * should be written afterward.
     */
    virtual void Align(unsigned align) = 0;

    /**
     * \brief Write data to the zone data without considering the current block or advancing the block position.
     * The data is written directly to the specified location instead of block memory.
     * \param dst The memory location to write data to.
     * \param size The amount of data to write.
     */
    virtual void* WriteDataRaw(const void* dst, size_t size) = 0;

    /**
     * \brief Write data with the current blocks write operation into its block memory.
     * Depending on the block type, the underlying stream might be written to or not.
     * The current block position is advanced by the number of bytes written.
     * The destination must be inside the current block's memory space, otherwise an exception is thrown.
     * \param dst The destination where the data is written to. Must be inside the current block's memory bounds.
     * \param size The amount of data to write.
     */
    virtual void* WriteDataInBlock(const void* dst, size_t size) = 0;
    virtual void IncBlockPos(size_t size) = 0;
    virtual void WriteNullTerminated(const void* dst) = 0;

    virtual bool ReusableShouldWrite(void** pPtr, size_t size, std::type_index type) = 0;
    virtual void ReusableAddOffset(void* ptr, size_t size, size_t count, std::type_index type) = 0;
    virtual void MarkFollowing(void** pPtr) = 0;

    template<typename T> bool ReusableShouldWrite(T** pPtr)
    {
        return ReusableShouldWrite(reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(pPtr)), sizeof(T), std::type_index(typeid(T)));
    }

    template<typename T> void ReusableAddOffset(T* ptr)
    {
        ReusableAddOffset(const_cast<void*>(reinterpret_cast<const void*>(ptr)), sizeof(T), 1, std::type_index(typeid(T)));
    }

    template<typename T> void ReusableAddOffset(T* ptr, const size_t count)
    {
        ReusableAddOffset(const_cast<void*>(reinterpret_cast<const void*>(ptr)), sizeof(T), count, std::type_index(typeid(T)));
    }

    template<typename T> T* Write(T* dst)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), sizeof(T)));
    }

    template<typename T> T* Write(T* dst, const size_t count)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), count * sizeof(T)));
    }

    template<typename T> T* WritePartial(T* dst, const size_t size)
    {
        return static_cast<T*>(WriteDataInBlock(reinterpret_cast<const void*>(dst), size));
    }

    template<typename T> void MarkFollowing(T*& ptr)
    {
        MarkFollowing(reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(&ptr)));
    }

    static std::unique_ptr<ZoneOutputStream>
        Create(unsigned pointerBitCount, unsigned blockBitCount, std::vector<XBlock*>& blocks, block_t insertBlock, InMemoryZoneData& zoneData);
};
