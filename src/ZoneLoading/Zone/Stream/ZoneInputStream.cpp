#include "ZoneInputStream.h"

#include "Loading/Exception/BlockOverflowException.h"
#include "Loading/Exception/InvalidOffsetBlockException.h"
#include "Loading/Exception/InvalidOffsetBlockOffsetException.h"
#include "Loading/Exception/OutOfBlockBoundsException.h"
#include "Utils/Alignment.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_map>

ZoneStreamFillReadAccessor::ZoneStreamFillReadAccessor(void* blockBuffer, const size_t bufferSize, const unsigned pointerByteCount, const size_t offset)
    : m_block_buffer(blockBuffer),
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
    return ZoneStreamFillReadAccessor(static_cast<char*>(m_block_buffer) + offset, m_buffer_size - offset, m_pointer_byte_count, m_offset + offset);
}

size_t ZoneStreamFillReadAccessor::Offset() const
{
    return m_offset;
}

void* ZoneStreamFillReadAccessor::BlockBuffer(const size_t offset) const
{
    return static_cast<uint8_t*>(m_block_buffer) + offset;
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
                          MemoryManager& memory,
                          std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
            : m_blocks(blocks),
              m_block_offsets(blocks.size()),
              m_stream(stream),
              m_memory(memory),
              m_pointer_byte_count(pointerBitCount / 8u),
              m_block_mask((std::numeric_limits<uintptr_t>::max() >> (sizeof(uintptr_t) * 8 - blockBitCount)) << (pointerBitCount - blockBitCount)),
              m_block_shift(pointerBitCount - blockBitCount),
              m_offset_mask(std::numeric_limits<uintptr_t>::max() >> (sizeof(uintptr_t) * 8 - (pointerBitCount - blockBitCount))),
              m_last_fill_size(0),
              m_has_progress_callback(false),
              m_progress_current_size(0uz),
              m_progress_total_size(0uz)
        {
            assert(pointerBitCount % 8u == 0u);
            assert(insertBlock < static_cast<block_t>(blocks.size()));

            std::memset(m_block_offsets.data(), 0, sizeof(decltype(m_block_offsets)::value_type) * m_block_offsets.size());

            m_insert_block = blocks[insertBlock];

            if (progressCallback)
            {
                m_has_progress_callback = true;
                m_progress_callback = *std::move(progressCallback);
                m_progress_total_size = CalculateTotalSize();
            }
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
            m_last_fill_size = size;

            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]];

                LoadDataFromBlock(*block, blockBufferForFill, size);

                return ZoneStreamFillReadAccessor(blockBufferForFill, size, m_pointer_byte_count, 0);
            }

            m_fill_buffer.resize(size);
            m_stream.Load(m_fill_buffer.data(), size);
            return ZoneStreamFillReadAccessor(m_fill_buffer.data(), size, m_pointer_byte_count, 0);
        }

        ZoneStreamFillReadAccessor AppendToFill(const size_t appendSize) override
        {
            const auto appendOffset = m_last_fill_size;
            m_last_fill_size += appendSize;

            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]] - appendOffset;
                LoadDataFromBlock(*block, &block->m_buffer[m_block_offsets[block->m_index]], appendSize);

                return ZoneStreamFillReadAccessor(blockBufferForFill, m_last_fill_size, m_pointer_byte_count, 0);
            }

            m_fill_buffer.resize(appendOffset + appendSize);
            m_stream.Load(m_fill_buffer.data() + appendOffset, appendSize);
            return ZoneStreamFillReadAccessor(m_fill_buffer.data(), m_last_fill_size, m_pointer_byte_count, 0);
        }

        ZoneStreamFillReadAccessor GetLastFill() override
        {
            assert(!m_fill_buffer.empty());

            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                auto* blockBufferForFill = &block->m_buffer[m_block_offsets[block->m_index]] - m_last_fill_size;

                return ZoneStreamFillReadAccessor(blockBufferForFill, m_last_fill_size, m_pointer_byte_count, 0);
            }

            assert(m_fill_buffer.size() == m_last_fill_size);
            return ZoneStreamFillReadAccessor(m_fill_buffer.data(), m_last_fill_size, m_pointer_byte_count, 0);
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

            const auto blockNum = m_insert_block->m_index;
            const auto blockOffset = static_cast<uintptr_t>(m_block_offsets[m_insert_block->m_index]);
            const auto zonePtr = (static_cast<uintptr_t>(blockNum) << m_block_shift) | (blockOffset & m_offset_mask);

            IncBlockPos(*m_insert_block, m_pointer_byte_count);

            return zonePtr;
        }

        void SetInsertedPointerAliasLookup(const uintptr_t zonePtr, void* value) override
        {
            assert((static_cast<block_t>((zonePtr & m_block_mask) >> m_block_shift)) < m_blocks.size());
            m_alias_redirect_lookup.emplace(zonePtr, value);
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

        void AddPointerLookup(void* alias, const void* blockPtr) override
        {
            assert(!m_block_stack.empty());
            const auto* block = m_block_stack.top();
            assert(blockPtr >= block->m_buffer.get() && blockPtr < block->m_buffer.get() + block->m_buffer_size);

            // Non-normal blocks cannot be referenced via zone pointer anyway
            if (block->m_type != XBlockType::BLOCK_TYPE_NORMAL)
                return;

            const auto blockNum = block->m_index;
            const auto blockOffset = static_cast<uintptr_t>(static_cast<const uint8_t*>(blockPtr) - block->m_buffer.get());
            const auto zonePtr = (static_cast<uintptr_t>(blockNum) << m_block_shift) | (blockOffset & m_offset_mask);
            m_pointer_redirect_lookup.emplace(zonePtr, alias);
        }

        MaybePointerFromLookup<void> ConvertOffsetToPointerLookup(const void* offset) override
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

            const auto foundPointerLookup = m_pointer_redirect_lookup.find(offsetInt);
            if (foundPointerLookup != m_pointer_redirect_lookup.end())
                return MaybePointerFromLookup<void>(foundPointerLookup->second);

            return MaybePointerFromLookup<void>(&block->m_buffer[blockOffset], blockNum, blockOffset);
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

            if (block->m_buffer_size <= blockOffset + sizeof(uintptr_t))
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            const auto foundAliasLookup = m_alias_redirect_lookup.find(offsetInt);
            if (foundAliasLookup != m_alias_redirect_lookup.end())
                return foundAliasLookup->second;

            const auto foundPointerLookup = m_pointer_redirect_lookup.find(offsetInt);
            if (foundPointerLookup != m_pointer_redirect_lookup.end())
                return *static_cast<void**>(foundPointerLookup->second);

            assert(false);
            throw InvalidOffsetBlockOffsetException(block, blockOffset);
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

            con::debug(ss.str());
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

            // We cannot know the full size of the temp block
            if (m_has_progress_callback && block.m_type != XBlockType::BLOCK_TYPE_TEMP)
            {
                m_progress_current_size += size;
                m_progress_callback->OnProgress(m_progress_current_size, m_progress_total_size);
            }
        }

        void Align(const XBlock& block, const unsigned align)
        {
            if (align > 0)
            {
                const auto blockIndex = block.m_index;
                m_block_offsets[blockIndex] = utils::Align(m_block_offsets[blockIndex], static_cast<size_t>(align));
            }
        }

        [[nodiscard]] size_t CalculateTotalSize() const
        {
            size_t result = 0uz;

            for (const auto& block : m_blocks)
            {
                // We cannot know the full size of the temp block
                if (block->m_type != XBlockType::BLOCK_TYPE_TEMP)
                    result += block->m_buffer_size;
            }

            return result;
        }

        std::vector<XBlock*>& m_blocks;
        std::vector<size_t> m_block_offsets;

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
        size_t m_last_fill_size;
        // These lookups map a block offset to a pointer in case of a platform mismatch
        std::unordered_map<uintptr_t, void*> m_pointer_redirect_lookup;
        std::unordered_map<uintptr_t, void*> m_alias_redirect_lookup;

        bool m_has_progress_callback;
        std::unique_ptr<ProgressCallback> m_progress_callback;
        size_t m_progress_current_size;
        size_t m_progress_total_size;
    };
} // namespace

std::unique_ptr<ZoneInputStream> ZoneInputStream::Create(const unsigned pointerBitCount,
                                                         const unsigned blockBitCount,
                                                         std::vector<XBlock*>& blocks,
                                                         const block_t insertBlock,
                                                         ILoadingStream& stream,
                                                         MemoryManager& memory,
                                                         std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
{
    return std::make_unique<XBlockInputStream>(pointerBitCount, blockBitCount, blocks, insertBlock, stream, memory, std::move(progressCallback));
}
