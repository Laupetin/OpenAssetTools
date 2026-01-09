#pragma once

#include "InMemoryZoneData.h"
#include "Zone/Stream/IZoneStream.h"
#include "Zone/XBlock.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <vector>

class ZoneOutputOffset
{
public:
    ZoneOutputOffset();
    explicit ZoneOutputOffset(void* offset);

    [[nodiscard]] ZoneOutputOffset AtOffset(size_t innerOffset) const;
    void Inc(size_t size);
    [[nodiscard]] void* Offset() const;

private:
    void* m_offset;
};

class ZoneStreamFillWriteAccessor
{
public:
    ZoneStreamFillWriteAccessor(void* blockBuffer, size_t bufferSize);

    [[nodiscard]] ZoneStreamFillWriteAccessor AtOffset(size_t offset) const;
    [[nodiscard]] ZoneOutputOffset Offset() const;

    template<typename T> void Fill(const T& value, const size_t offset) const
    {
        assert(offset + sizeof(T) <= m_buffer_size);

        *reinterpret_cast<T*>(static_cast<char*>(m_block_buffer) + offset) = value;
    }

    template<typename T, size_t S> void FillArray(T (&value)[S], const size_t offset) const
    {
        assert(offset + sizeof(T) * S <= m_buffer_size);

        std::memcpy(static_cast<char*>(m_block_buffer) + offset, value, sizeof(T) * S);
    }

private:
    void* m_block_buffer;
    size_t m_buffer_size;
};

class ZoneOutputStream : public IZoneStream
{
public:
    /**
     * \brief Returns the configured bits that make up a pointer.
     */
    [[nodiscard]] virtual unsigned GetPointerByteCount() const = 0;

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
    virtual ZoneOutputOffset WriteDataRaw(const void* dst, size_t size) = 0;

    /**
     * \brief Write data with the current blocks write operation into its block memory.
     * Depending on the block type, the underlying stream might be written to or not.
     * The current block position is advanced by the number of bytes written.
     * The destination must be inside the current block's memory space, otherwise an exception is thrown.
     * \param dst The destination where the data is written to. Must be inside the current block's memory bounds.
     * \param size The amount of data to write.
     */
    virtual ZoneOutputOffset WriteDataInBlock(const void* dst, size_t size) = 0;
    virtual void IncBlockPos(size_t size) = 0;
    virtual void WriteNullTerminated(const void* dst) = 0;

    virtual ZoneStreamFillWriteAccessor WriteWithFill(size_t size) = 0;

    virtual bool ReusableShouldWrite(void* pPtr, ZoneOutputOffset outputOffset, size_t size, std::type_index type) = 0;
    virtual void ReusableAddOffset(void* ptr, size_t size, size_t count, std::type_index type) = 0;
    virtual void MarkFollowing(ZoneOutputOffset offset) = 0;

    template<typename T> bool ReusableShouldWrite(T* pPtr, const ZoneOutputOffset outputOffset)
    {
        return ReusableShouldWrite(const_cast<void*>(reinterpret_cast<const void*>(pPtr)), outputOffset, sizeof(T), std::type_index(typeid(T)));
    }

    template<typename T> void ReusableAddOffset(T* ptr)
    {
        ReusableAddOffset(const_cast<void*>(reinterpret_cast<const void*>(ptr)), sizeof(T), 1, std::type_index(typeid(T)));
    }

    template<typename T> void ReusableAddOffset(T* ptr, const size_t count)
    {
        ReusableAddOffset(const_cast<void*>(reinterpret_cast<const void*>(ptr)), sizeof(T), count, std::type_index(typeid(T)));
    }

    template<typename T> ZoneOutputOffset Write(T* dst)
    {
        return WriteDataInBlock(reinterpret_cast<const void*>(dst), sizeof(T));
    }

    template<typename T> ZoneOutputOffset Write(T* dst, const size_t count)
    {
        return WriteDataInBlock(reinterpret_cast<const void*>(dst), count * sizeof(T));
    }

    ZoneOutputOffset WritePartial(const void* dst, const size_t size)
    {
        return WriteDataInBlock(dst, size);
    }

    static std::unique_ptr<ZoneOutputStream>
        Create(unsigned pointerBitCount, unsigned blockBitCount, std::vector<XBlock*>& blocks, block_t insertBlock, InMemoryZoneData& zoneData);
};
