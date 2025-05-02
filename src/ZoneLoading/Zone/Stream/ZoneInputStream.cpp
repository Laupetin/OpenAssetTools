#include "ZoneInputStream.h"

#include "Loading/Exception/BlockOverflowException.h"
#include "Loading/Exception/InvalidOffsetBlockException.h"
#include "Loading/Exception/InvalidOffsetBlockOffsetException.h"
#include "Loading/Exception/OutOfBlockBoundsException.h"
#include "Utils/Alignment.h"

#include <cassert>
#include <cstring>
#include <stack>

namespace
{
    class XBlockInputStream final : public ZoneInputStream
    {
    public:
        XBlockInputStream(std::vector<XBlock*>& blocks, ILoadingStream& stream, const unsigned blockBitCount, const block_t insertBlock)
            : m_blocks(blocks),
              m_stream(stream)
        {
            const auto blockCount = static_cast<unsigned>(blocks.size());
            m_block_offsets = std::make_unique<size_t[]>(blockCount);
            std::memset(m_block_offsets.get(), 0, sizeof(size_t) * blockCount);

            m_block_bit_count = blockBitCount;

            assert(insertBlock < static_cast<block_t>(blocks.size()));
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
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return;

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
                memset(dst, 0, size);
                break;

            case XBlock::Type::BLOCK_TYPE_DELAY:
                assert(false);
                break;
            }

            IncBlockPos(size);
        }

        void IncBlockPos(const size_t size) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return;

            const auto* block = m_block_stack.top();
            m_block_offsets[block->m_index] += size;
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

        void** InsertPointer() override
        {
            m_block_stack.push(m_insert_block);

            Align(alignof(void*));

            if (m_block_offsets[m_insert_block->m_index] + sizeof(void*) > m_insert_block->m_buffer_size)
                throw BlockOverflowException(m_insert_block);

            auto* ptr = reinterpret_cast<void**>(&m_insert_block->m_buffer[m_block_offsets[m_insert_block->m_index]]);

            IncBlockPos(sizeof(void*));

            m_block_stack.pop();

            return ptr;
        }

        void* ConvertOffsetToPointer(const void* offset) override
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

        void* ConvertOffsetToAlias(const void* offset) override
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

        unsigned m_block_bit_count;
        XBlock* m_insert_block;
    };
} // namespace

std::unique_ptr<ZoneInputStream> ZoneInputStream::Create(std::vector<XBlock*>& blocks, ILoadingStream& stream, unsigned blockBitCount, block_t insertBlock)
{
    return std::make_unique<XBlockInputStream>(blocks, stream, blockBitCount, insertBlock);
}
