#include "ZoneInputStream.h"

#include "Loading/Exception/BlockOverflowException.h"
#include "Loading/Exception/InvalidOffsetBlockException.h"
#include "Loading/Exception/InvalidOffsetBlockOffsetException.h"
#include "Loading/Exception/OutOfBlockBoundsException.h"
#include "Utils/Alignment.h"

#include <cassert>
#include <cstring>
#include <stack>

ZoneStreamFillReadAccessor::ZoneStreamFillReadAccessor(const void* buffer, const size_t bufferSize, const unsigned pointerByteCount)
    : m_buffer(buffer),
      m_buffer_size(bufferSize),
      m_pointer_byte_count(pointerByteCount)
{
}

ZoneStreamFillReadAccessor ZoneStreamFillReadAccessor::AtOffset(const size_t offset) const
{
    assert(offset < m_buffer_size);
    return ZoneStreamFillReadAccessor(static_cast<const char*>(m_buffer) + offset, m_buffer_size - offset, m_pointer_byte_count);
}

namespace
{
    class XBlockInputStream final : public ZoneInputStream
    {
    public:
        XBlockInputStream(
            const unsigned pointerBitCount, const unsigned blockBitCount, std::vector<XBlock*>& blocks, const block_t insertBlock, ILoadingStream& stream)
            : m_blocks(blocks),
              m_stream(stream),
              m_pointer_byte_count(pointerBitCount / 8u),
              m_block_bit_count(blockBitCount)
        {
            assert(pointerBitCount % 8u == 0u);
            assert(insertBlock < static_cast<block_t>(blocks.size()));

            const auto blockCount = static_cast<unsigned>(blocks.size());
            m_block_offsets = std::make_unique<size_t[]>(blockCount);
            std::memset(m_block_offsets.get(), 0, sizeof(size_t) * blockCount);

            m_insert_block = blocks[insertBlock];
        }

        void PushBlock(const block_t block) override
        {
            assert(block < static_cast<block_t>(m_blocks.size()));

            auto* newBlock = m_blocks[block];
            assert(newBlock->m_index == block);

            m_block_stack.push(newBlock);

            if (newBlock->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
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
            if (poppedBlock->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
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

            Align(align);

            if (m_block_offsets[block->m_index] > block->m_buffer_size)
                throw BlockOverflowException(block);

            return &block->m_buffer[m_block_offsets[block->m_index]];
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

                if (block->m_buffer > dst || block->m_buffer + block->m_buffer_size < dst)
                    throw OutOfBlockBoundsException(block);

                if (static_cast<uint8_t*>(dst) + size > block->m_buffer + block->m_buffer_size)
                    throw BlockOverflowException(block);

                // Theoretically ptr should always be at the current block offset.
                assert(dst == &block->m_buffer[m_block_offsets[block->m_index]]);

                switch (block->m_type)
                {
                case XBlock::Type::BLOCK_TYPE_TEMP:
                case XBlock::Type::BLOCK_TYPE_NORMAL:
                    m_stream.Load(dst, size);
                    break;

                case XBlock::Type::BLOCK_TYPE_RUNTIME:
                    std::memset(dst, 0, size);
                    break;

                case XBlock::Type::BLOCK_TYPE_DELAY:
                    assert(false);
                    break;
                }

                IncBlockPos(size);
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

            if (block->m_buffer > dst || block->m_buffer + block->m_buffer_size < dst)
                throw OutOfBlockBoundsException(block);

            // Theoretically ptr should always be at the current block offset.
            assert(dst == &block->m_buffer[m_block_offsets[block->m_index]]);

            uint8_t byte;
            auto offset = static_cast<size_t>(static_cast<uint8_t*>(dst) - block->m_buffer);
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
            m_fill_buffer.reserve(size);
            auto* dst = m_fill_buffer.data();

            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();
                switch (block->m_type)
                {
                case XBlock::Type::BLOCK_TYPE_TEMP:
                case XBlock::Type::BLOCK_TYPE_NORMAL:
                    m_stream.Load(dst, size);
                    break;

                case XBlock::Type::BLOCK_TYPE_RUNTIME:
                    std::memset(dst, 0, size);
                    break;

                case XBlock::Type::BLOCK_TYPE_DELAY:
                    assert(false);
                    break;
                }

                IncBlockPos(size);
            }
            else
            {
                m_stream.Load(dst, size);
            }

            return ZoneStreamFillReadAccessor(dst, size, m_pointer_byte_count);
        }

        void* InsertPointer() override
        {
            m_block_stack.push(m_insert_block);

            // Alignment of pointer should always be its size
            Align(m_pointer_byte_count);

            if (m_block_offsets[m_insert_block->m_index] + m_pointer_byte_count > m_insert_block->m_buffer_size)
                throw BlockOverflowException(m_insert_block);

            auto* ptr = static_cast<void*>(&m_insert_block->m_buffer[m_block_offsets[m_insert_block->m_index]]);

            IncBlockPos(m_pointer_byte_count);

            m_block_stack.pop();

            return ptr;
        }

        void* ConvertOffsetToPointerNative(const void* offset) override
        {
            // -1 because otherwise Block 0 Offset 0 would be just 0 which is already used to signalize a nullptr.
            // So all offsets are moved by 1.
            const auto offsetInt = reinterpret_cast<uintptr_t>(offset) - 1u;

            const auto blockNum = static_cast<block_t>(offsetInt >> (sizeof(offsetInt) * 8u - m_block_bit_count));
            const auto blockOffset = static_cast<size_t>(offsetInt & (UINTPTR_MAX >> m_block_bit_count));

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

            const auto blockNum = static_cast<block_t>(offsetInt >> (sizeof(offsetInt) * 8u - m_block_bit_count));
            const auto blockOffset = static_cast<size_t>(offsetInt & (UINTPTR_MAX >> m_block_bit_count));

            if (blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
                throw InvalidOffsetBlockException(blockNum);

            auto* block = m_blocks[blockNum];

            if (block->m_buffer_size <= blockOffset + sizeof(void*))
                throw InvalidOffsetBlockOffsetException(block, blockOffset);

            return *reinterpret_cast<void**>(&block->m_buffer[blockOffset]);
        }

    private:
        void IncBlockPos(const size_t size)
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return;

            const auto* block = m_block_stack.top();
            m_block_offsets[block->m_index] += size;
        }

        void Align(const unsigned align)
        {
            assert(!m_block_stack.empty());

            if (align > 0)
            {
                const auto blockIndex = m_block_stack.top()->m_index;
                m_block_offsets[blockIndex] = utils::Align(m_block_offsets[blockIndex], static_cast<size_t>(align));
            }
        }

        std::vector<XBlock*>& m_blocks;
        std::unique_ptr<size_t[]> m_block_offsets;

        std::stack<XBlock*> m_block_stack;
        std::stack<size_t> m_temp_offsets;
        ILoadingStream& m_stream;

        unsigned m_pointer_byte_count;
        unsigned m_block_bit_count;
        XBlock* m_insert_block;

        std::vector<uint8_t> m_fill_buffer;
    };
} // namespace

std::unique_ptr<ZoneInputStream> ZoneInputStream::Create(
    const unsigned pointerBitCount, const unsigned blockBitCount, std::vector<XBlock*>& blocks, const block_t insertBlock, ILoadingStream& stream)
{
    return std::make_unique<XBlockInputStream>(pointerBitCount, blockBitCount, blocks, insertBlock, stream);
}
