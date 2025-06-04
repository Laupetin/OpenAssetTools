#include "ZoneInputStream.h"

#include "Loading/Exception/BlockOverflowException.h"
#include "Loading/Exception/InvalidAliasLookupException.h"
#include "Loading/Exception/InvalidOffsetBlockException.h"
#include "Loading/Exception/InvalidOffsetBlockOffsetException.h"
#include "Loading/Exception/OutOfBlockBoundsException.h"
#include "Utils/Alignment.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>

ZoneStreamFillReadAccessor::ZoneStreamFillReadAccessor(
    const void* dataBuffer, void* blockBuffer, const size_t bufferSize, const unsigned pointerByteCount, const size_t offset)
    : m_data_buffer(dataBuffer),
      m_block_buffer(blockBuffer),
      m_buffer_size(bufferSize),
      m_pointer_byte_count(pointerByteCount),
      m_offset(offset)
{
    // Otherwise we cannot insert alias
    assert(m_pointer_byte_count <= sizeof(uintptr_t));
}

ZoneStreamFillReadAccessor ZoneStreamFillReadAccessor::AtOffset(const size_t offset) const
{
    assert(offset <= m_buffer_size);
    return ZoneStreamFillReadAccessor(static_cast<const char*>(m_data_buffer) + offset,
                                      static_cast<char*>(m_block_buffer) + offset,
                                      m_buffer_size - offset,
                                      m_pointer_byte_count,
                                      m_offset + offset);
}

size_t ZoneStreamFillReadAccessor::Offset() const
{
    return m_offset;
}

void ZoneStreamFillReadAccessor::InsertPointerRedirect(const uintptr_t aliasValue, const size_t offset) const
{
    // Memory should be zero by default
    if (aliasValue == 0)
        return;

    assert(offset < m_buffer_size);
    assert(m_block_buffer);

    std::memcpy(static_cast<char*>(m_block_buffer) + offset, &aliasValue, m_pointer_byte_count);
}

namespace
{
    class XBlockInputStream final : public ZoneInputStream
    {
    public:
        XBlockInputStream(const unsigned pointerBitCount,
                          const unsigned blockBitCount,
                          std::vector<XBlock*>& blocks,
                          const block_t insertBlock,
                          ILoadingStream& stream,
                          MemoryManager& memory)
            : m_blocks(blocks),
              m_stream(stream),
              m_memory(memory),
              m_pointer_byte_count(pointerBitCount / 8u),
              m_block_mask((std::numeric_limits<uintptr_t>::max() >> (sizeof(uintptr_t) * 8 - blockBitCount)) << (pointerBitCount - blockBitCount)),
              m_block_shift(pointerBitCount - blockBitCount),
              m_offset_mask(std::numeric_limits<uintptr_t>::max() >> (sizeof(uintptr_t) * 8 - (pointerBitCount - blockBitCount))),
              m_alias_mask(1uz << (pointerBitCount - 1uz))
        {
            assert(pointerBitCount % 8u == 0u);
            assert(insertBlock < static_cast<block_t>(blocks.size()));

            const auto blockCount = static_cast<unsigned>(blocks.size());
            m_block_offsets = std::make_unique<size_t[]>(blockCount);
            std::memset(m_block_offsets.get(), 0, sizeof(size_t) * blockCount);

            m_insert_block = blocks[insertBlock];
        }

        [[nodiscard]] unsigned GetPointerBitCount() const override
        {
            return m_pointer_byte_count * 8u;
        }

        void PushBlock(const block_t block) override
        {
            assert(block < static_cast<block_t>(m_blocks.size()));

            auto* newBlock = m_blocks[block];
            assert(newBlock->m_index == block);

            m_block_stack.push(newBlock);

            if (newBlock->m_type == XBlockType::BLOCK_TYPE_TEMP)
                m_temp_offsets.push(m_block_offsets[newBlock->m_index]);
        }

        block_t PopBlock() override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return -1;

            const auto* poppedBlock = m_block_stack.top();

            m_block_stack.pop();

            // If the temp block is not used anymore right now, reset it to the buffer start since as the name suggests, the data inside is temporary.
            if (poppedBlock->m_type == XBlockType::BLOCK_TYPE_TEMP)
            {
                m_block_offsets[poppedBlock->m_index] = m_temp_offsets.top();
                m_temp_offsets.pop();
            }

            return poppedBlock->m_index;
        }

        void* Alloc(const unsigned align) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return nullptr;

            auto* block = m_block_stack.top();

            Align(*block, align);

            if (m_block_offsets[block->m_index] > block->m_buffer_size)
                throw BlockOverflowException(block);

            return &block->m_buffer[m_block_offsets[block->m_index]];
        }

        void* AllocOutOfBlock(const unsigned align, const size_t size) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return nullptr;

            auto* block = m_block_stack.top();

            Align(*block, align);

            if (m_block_offsets[block->m_index] > block->m_buffer_size)
                throw BlockOverflowException(block);

            return m_memory.AllocRaw(size);
        }

        void LoadDataRaw(void* dst, const size_t size) override
        {
            m_stream.Load(dst, size);
        }

        void LoadDataInBlock(void* dst, const size_t size) override
        {
            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                auto* block = m_block_stack.top();

                if (block->m_buffer.get() > dst || block->m_buffer.get() + block->m_buffer_size < dst)
                    throw OutOfBlockBoundsException(block);

                if (static_cast<uint8_t*>(dst) + size > block->m_buffer.get() + block->m_buffer_size)
                    throw BlockOverflowException(block);

                // Theoretically ptr should always be at the current block offset.
                assert(dst == &block->m_buffer[m_block_offsets[block->m_index]]);

                LoadDataFromBlock(*block, dst, size);
            }
            else
            {
                m_stream.Load(dst, size);
            }
        }

        void LoadNullTerminated(void* dst) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return;

            auto* block = m_block_stack.top();

            if (block->m_buffer.get() > dst || block->m_buffer.get() + block->m_buffer_size < dst)
                throw OutOfBlockBoundsException(block);

            // Theoretically ptr should always be at the current block offset.
            assert(dst == &block->m_buffer[m_block_offsets[block->m_index]]);

            uint8_t byte;
            auto offset = static_cast<size_t>(static_cast<uint8_t*>(dst) - block->m_buffer.get());
            do
            {
                if (offset >= block->m_buffer_size)
                    throw BlockOverflowException(block);

                m_stream.Load(&byte, 1);
                block->m_buffer[offset++] = byte;
            } while (byte != 0);

            m_block_offsets[block->m_index] = offset;
        }

        ZoneStreamFillReadAccessor LoadWithFill(const size_t size) override
        {
            m_fill_buffer.resize(size);
            auto* dst = m_fill_buffer.data();

            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]];

                LoadDataFromBlock(*block, dst, size);

                return ZoneStreamFillReadAccessor(dst, blockBufferForFill, size, m_pointer_byte_count, 0);
            }

            m_stream.Load(dst, size);
            return ZoneStreamFillReadAccessor(dst, nullptr, size, m_pointer_byte_count, 0);
        }

        ZoneStreamFillReadAccessor AppendToFill(const size_t appendSize) override
        {
            const auto appendOffset = m_fill_buffer.size();
            m_fill_buffer.resize(appendOffset + appendSize);
            auto* dst = m_fill_buffer.data() + appendOffset;

            const auto newTotalSize = appendOffset + appendSize;
            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]] - appendOffset;

                LoadDataFromBlock(*block, dst, appendSize);

                return ZoneStreamFillReadAccessor(m_fill_buffer.data(), blockBufferForFill, newTotalSize, m_pointer_byte_count, 0);
            }

            m_stream.Load(dst, appendSize);
            return ZoneStreamFillReadAccessor(m_fill_buffer.data(), nullptr, newTotalSize, m_pointer_byte_count, 0);
        }

        ZoneStreamFillReadAccessor GetLastFill() override
        {
            assert(!m_fill_buffer.empty());

            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]] - m_fill_buffer.size();

                return ZoneStreamFillReadAccessor(m_fill_buffer.data(), blockBufferForFill, m_fill_buffer.size(), m_pointer_byte_count, 0);
            }

            return ZoneStreamFillReadAccessor(m_fill_buffer.data(), nullptr, m_fill_buffer.size(), m_pointer_byte_count, 0);
        }

        void* InsertPointerNative() override
        {
            // Alignment of pointer should always be its size
            Align(*m_insert_block, m_pointer_byte_count);

            if (m_block_offsets[m_insert_block->m_index] + m_pointer_byte_count > m_insert_block->m_buffer_size)
                throw BlockOverflowException(m_insert_block);

            auto* ptr = static_cast<void*>(&m_insert_block->m_buffer[m_block_offsets[m_insert_block->m_index]]);

            IncBlockPos(*m_insert_block, m_pointer_byte_count);

            return ptr;
        }

        uintptr_t InsertPointerAliasLookup() override
        {
            // Alignment of pointer should always be its size
            Align(*m_insert_block, m_pointer_byte_count);

            if (m_block_offsets[m_insert_block->m_index] + m_pointer_byte_count > m_insert_block->m_buffer_size)
                throw BlockOverflowException(m_insert_block);

            auto* ptr = static_cast<void*>(&m_insert_block->m_buffer[m_block_offsets[m_insert_block->m_index]]);

            IncBlockPos(*m_insert_block, m_pointer_byte_count);

            const auto newLookupIndex = static_cast<uintptr_t>(m_alias_lookup.size()) | m_alias_mask;
            m_alias_lookup.emplace_back(nullptr);

            std::memcpy(ptr, &newLookupIndex, m_pointer_byte_count);

            return newLookupIndex;
        }

        void SetInsertedPointerAliasLookup(const uintptr_t lookupEntry, void* value) override
        {
            assert(lookupEntry & m_alias_mask);

            const auto aliasIndex = lookupEntry & ~m_alias_mask;

            assert(aliasIndex < m_alias_lookup.size());

            m_alias_lookup[aliasIndex] = value;
        }

        void* ConvertOffsetToPointerNative(const void* offset) override
        {
            // -1 because otherwise Block 0 Offset 0 would be just 0 which is already used to signalize a nullptr.
            // So all offsets are moved by 1.
            const auto offsetInt = reinterpret_cast<uintptr_t>(offset) - 1u;

            const auto blockNum = static_cast<block_t>((offsetInt & m_block_mask) >> m_block_shift);
            const auto blockOffset = static_cast<size_t>(offsetInt & m_offset_mask);

            if (blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
                throw InvalidOffsetBlockException(blockNum);

            auto* block = m_blocks[blockNum];

            if (block->m_buffer_size <= blockOffset)
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            return &block->m_buffer[blockOffset];
        }

        void* ConvertOffsetToAliasNative(const void* offset) override
        {
            // For details see ConvertOffsetToPointer
            const auto offsetInt = reinterpret_cast<uintptr_t>(offset) - 1u;

            const auto blockNum = static_cast<block_t>((offsetInt & m_block_mask) >> m_block_shift);
            const auto blockOffset = static_cast<size_t>(offsetInt & m_offset_mask);

            if (blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
                throw InvalidOffsetBlockException(blockNum);

            auto* block = m_blocks[blockNum];

            if (block->m_buffer_size <= blockOffset + sizeof(void*))
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            return *reinterpret_cast<void**>(&block->m_buffer[blockOffset]);
        }

        uintptr_t AllocRedirectEntry(void** alias) override
        {
            // nullptr is always lookup alias 0
            if (*alias == nullptr)
                return 0;

            const auto newIndex = m_pointer_redirect_lookup.size();

            m_pointer_redirect_lookup.emplace_back(alias);

            return static_cast<uintptr_t>(newIndex + 1);
        }

        void* ConvertOffsetToPointerRedirect(const void* offset) override
        {
            // For details see ConvertOffsetToPointer
            const auto offsetInt = reinterpret_cast<uintptr_t>(offset) - 1u;

            const auto blockNum = static_cast<block_t>((offsetInt & m_block_mask) >> m_block_shift);
            const auto blockOffset = static_cast<size_t>(offsetInt & m_offset_mask);

            if (blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
                throw InvalidOffsetBlockException(blockNum);

            auto* block = m_blocks[blockNum];

            if (block->m_buffer_size <= blockOffset + sizeof(void*))
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            uintptr_t lookupEntry = 0u;
            std::memcpy(&lookupEntry, &block->m_buffer[blockOffset], m_pointer_byte_count);

            if (lookupEntry == 0)
                return nullptr;
            if (lookupEntry > m_pointer_redirect_lookup.size())
                throw InvalidAliasLookupException(lookupEntry - 1, m_pointer_redirect_lookup.size());

            return *m_pointer_redirect_lookup[lookupEntry - 1];
        }

        void* ConvertOffsetToAliasLookup(const void* offset) override
        {
            // For details see ConvertOffsetToPointer
            const auto offsetInt = reinterpret_cast<uintptr_t>(offset) - 1u;

            const auto blockNum = static_cast<block_t>((offsetInt & m_block_mask) >> m_block_shift);
            const auto blockOffset = static_cast<size_t>(offsetInt & m_offset_mask);

            if (blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
                throw InvalidOffsetBlockException(blockNum);

            auto* block = m_blocks[blockNum];

            if (block->m_buffer_size <= blockOffset + sizeof(void*))
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            uintptr_t lookupEntry = 0u;
            std::memcpy(&lookupEntry, &block->m_buffer[blockOffset], m_pointer_byte_count);

            if (lookupEntry == 0)
                return nullptr;

            if (lookupEntry & m_alias_mask)
            {
                const auto aliasIndex = lookupEntry & ~m_alias_mask;

                if (aliasIndex >= m_alias_lookup.size())
                    throw InvalidAliasLookupException(aliasIndex, m_alias_lookup.size());

                return m_alias_lookup[aliasIndex];
            }

            const auto redirectIndex = lookupEntry - 1;

            if (redirectIndex >= m_pointer_redirect_lookup.size())
                throw InvalidAliasLookupException(redirectIndex, m_pointer_redirect_lookup.size());

            return *m_pointer_redirect_lookup[redirectIndex];
        }

#ifdef DEBUG_OFFSETS
        void DebugOffsets(const size_t assetIndex) const override
        {
            std::ostringstream ss;

            ss << "Asset " << assetIndex;
            for (const auto& block : m_blocks)
            {
                if (block->m_type != XBlockType::BLOCK_TYPE_NORMAL)
                    continue;

                ss << " " << m_block_offsets[block->m_index];
            }

            ss << "\n";
            std::cout << ss.str();
        }
#endif

    private:
        void LoadDataFromBlock(const XBlock& block, void* dst, const size_t size)
        {
            switch (block.m_type)
            {
            case XBlockType::BLOCK_TYPE_TEMP:
            case XBlockType::BLOCK_TYPE_NORMAL:
                m_stream.Load(dst, size);
                break;

            case XBlockType::BLOCK_TYPE_RUNTIME:
                std::memset(dst, 0, size);
                break;

            case XBlockType::BLOCK_TYPE_DELAY:
                assert(false);
                break;
            }

            IncBlockPos(block, size);
        }

        void IncBlockPos(const XBlock& block, const size_t size)
        {
            m_block_offsets[block.m_index] += size;
        }

        void Align(const XBlock& block, const unsigned align)
        {
            if (align > 0)
            {
                const auto blockIndex = block.m_index;
                m_block_offsets[blockIndex] = utils::Align(m_block_offsets[blockIndex], static_cast<size_t>(align));
            }
        }

        std::vector<XBlock*>& m_blocks;
        std::unique_ptr<size_t[]> m_block_offsets;

        std::stack<XBlock*> m_block_stack;
        std::stack<size_t> m_temp_offsets;
        ILoadingStream& m_stream;

        MemoryManager& m_memory;

        unsigned m_pointer_byte_count;
        uintptr_t m_block_mask;
        unsigned m_block_shift;
        uintptr_t m_offset_mask;
        XBlock* m_insert_block;

        std::vector<uint8_t> m_fill_buffer;
        std::vector<void**> m_pointer_redirect_lookup;
        std::vector<void*> m_alias_lookup;
        size_t m_alias_mask;
    };
} // namespace

std::unique_ptr<ZoneInputStream> ZoneInputStream::Create(const unsigned pointerBitCount,
                                                         const unsigned blockBitCount,
                                                         std::vector<XBlock*>& blocks,
                                                         const block_t insertBlock,
                                                         ILoadingStream& stream,
                                                         MemoryManager& memory)
{
    return std::make_unique<XBlockInputStream>(pointerBitCount, blockBitCount, blocks, insertBlock, stream, memory);
}
