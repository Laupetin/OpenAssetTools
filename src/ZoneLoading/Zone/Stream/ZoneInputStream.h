#pragma once

#include "Loading/ILoadingStream.h"
#include "Utils/MemoryManager.h"
#include "Zone/Stream/IZoneStream.h"
#include "Zone/XBlock.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

class ZoneStreamFillReadAccessor
{
public:
    ZoneStreamFillReadAccessor(const void* dataBuffer, void* blockBuffer, size_t bufferSize, unsigned pointerByteCount, size_t offset);

    [[nodiscard]] ZoneStreamFillReadAccessor AtOffset(size_t offset) const;
    [[nodiscard]] size_t Offset() const;

    template<typename T> void Fill(T& value, const size_t offset) const
    {
        assert(offset + sizeof(T) <= m_buffer_size);

        value = *reinterpret_cast<const T*>(static_cast<const char*>(m_data_buffer) + offset);
    }

    template<typename T, size_t S> void FillArray(T (&value)[S], const size_t offset) const
    {
        assert(offset + sizeof(T) * S <= m_buffer_size);

        std::memcpy(value, static_cast<const char*>(m_data_buffer) + offset, sizeof(T) * S);
    }

    template<typename T> void FillPtr(T*& value, const size_t offset) const
    {
        assert(offset + m_pointer_byte_count <= m_buffer_size);
        assert(m_pointer_byte_count <= sizeof(uintptr_t));

        value = nullptr;
        std::memcpy(&value, static_cast<const char*>(m_data_buffer) + offset, m_pointer_byte_count);
    }

    void InsertPointerRedirect(uintptr_t aliasValue, size_t offset) const;

private:
    const void* m_data_buffer;
    void* m_block_buffer;
    size_t m_buffer_size;
    unsigned m_pointer_byte_count;
    size_t m_offset;
};

class ZoneInputStream : public IZoneStream
{
public:
    /**
     * \brief Returns the configured bits that make up a pointer.
     */
    [[nodiscard]] virtual unsigned GetPointerBitCount() const = 0;

    /**
     * \brief Retrieves the new read position in the current block by aligning the position with the specified value and then returning the current read
     * position in the block.
     * \param align The alignment value that the read position is aligned with before being returned. This should typically be the alignment of the struct that
     * should be read afterward.
     * \return A pointer to the memory in which following load calls will load data to.
     */
    virtual void* Alloc(unsigned align) = 0;

    /**
     * \copydoc ZoneInputStream#Alloc(unsigned)
     */
    template<typename T> T* Alloc(const unsigned align)
    {
        return static_cast<T*>(Alloc(align));
    }

    virtual void* AllocOutOfBlock(unsigned align, size_t size) = 0;

    /**
     * \copydoc ZoneInputStream#AllocOutOfBlock(unsigned)
     */
    template<typename T> T* AllocOutOfBlock(const unsigned align, const size_t arraySize = 1u)
    {
        return static_cast<T*>(AllocOutOfBlock(align, sizeof(T) * arraySize));
    }

    /**
     * \brief Loads data from the underlying stream without considering the current block or advancing the block position.
     * The data is read directly to the specified location instead of block memory.
     * \param dst The memory location to load data to.
     * \param size The amount of data to load.
     */
    virtual void LoadDataRaw(void* dst, size_t size) = 0;

    /**
     * \brief Loads data with the current blocks read operation into its block memory.
     * Depending on the block type, the underlying stream might be read from or not.
     * The current block position is advanced by the number of bytes read.
     * The destination must be inside the current block's memory space, otherwise an exception is thrown.
     * \param dst The destination where the data is loaded to. Must be inside the current block's memory bounds.
     * \param size The amount of data to load.
     */
    virtual void LoadDataInBlock(void* dst, size_t size) = 0;
    virtual void LoadNullTerminated(void* dst) = 0;
    virtual ZoneStreamFillReadAccessor LoadWithFill(size_t size) = 0;
    virtual ZoneStreamFillReadAccessor AppendToFill(size_t appendSize) = 0;
    virtual ZoneStreamFillReadAccessor GetLastFill() = 0;

    template<typename T> void Load(T* dst)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), sizeof(T));
    }

    template<typename T> void Load(T* dst, const size_t count)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), count * sizeof(T));
    }

    template<typename T> void LoadPartial(T* dst, const size_t size)
    {
        LoadDataInBlock(const_cast<void*>(reinterpret_cast<const void*>(dst)), size);
    }

    virtual void* InsertPointerNative() = 0;

    template<typename T> T** InsertPointerNative()
    {
        return static_cast<T**>(InsertPointerNative());
    }

    virtual uintptr_t InsertPointerAliasLookup() = 0;

    virtual void SetInsertedPointerAliasLookup(uintptr_t lookupEntry, void* value) = 0;

    virtual void* ConvertOffsetToPointerNative(const void* offset) = 0;

    template<typename T> T* ConvertOffsetToPointerNative(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToPointerNative(static_cast<const void*>(offset)));
    }

    virtual void* ConvertOffsetToAliasNative(const void* offset) = 0;

    template<typename T> T* ConvertOffsetToAliasNative(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToAliasNative(static_cast<const void*>(offset)));
    }

    virtual uintptr_t AllocRedirectEntry(void** alias) = 0;

    template<typename T> uintptr_t AllocRedirectEntry(T*& offset)
    {
        return AllocRedirectEntry(reinterpret_cast<void**>(const_cast<std::remove_const_t<T>**>(&offset)));
    }

    virtual void* ConvertOffsetToPointerRedirect(const void* offset) = 0;

    template<typename T> T* ConvertOffsetToPointerRedirect(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToPointerRedirect(static_cast<const void*>(offset)));
    }

    virtual void* ConvertOffsetToAliasLookup(const void* offset) = 0;

    template<typename T> T* ConvertOffsetToAliasLookup(T* offset)
    {
        return static_cast<T*>(ConvertOffsetToAliasLookup(static_cast<const void*>(offset)));
    }

    static std::unique_ptr<ZoneInputStream> Create(
        unsigned pointerBitCount, unsigned blockBitCount, std::vector<XBlock*>& blocks, block_t insertBlock, ILoadingStream& stream, MemoryManager& memory);
};
