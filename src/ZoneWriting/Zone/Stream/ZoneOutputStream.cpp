#include "ZoneOutputStream.h"

#include "InMemoryZoneData.h"
#include "Utils/Alignment.h"
#include "Zone/XBlock.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace
{
    class ReusableEntry
    {
    public:
        ReusableEntry(void* startPtr, const size_t entrySize, const size_t entryCount, const uintptr_t startZonePtr)
            : m_start_ptr(startPtr),
              m_end_ptr(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(startPtr) + entrySize * entryCount)),
              m_start_zone_ptr(startZonePtr),
              m_entry_size(entrySize),
              m_entry_count(entryCount)
        {
        }

        void* m_start_ptr;
        void* m_end_ptr;
        uintptr_t m_start_zone_ptr;
        size_t m_entry_size;
        size_t m_entry_count;
    };

    class InMemoryZoneOutputStream final : public ZoneOutputStream
    {
    public:
        InMemoryZoneOutputStream(
            const unsigned pointerBitCount, const unsigned blockBitCount, std::vector<XBlock*>& blocks, const block_t insertBlock, InMemoryZoneData& zoneData)
            : m_zone_data(zoneData),
              m_blocks(blocks),
              m_block_bit_count(blockBitCount),
              m_pointer_byte_count(pointerBitCount / 8u),

              // -1
              m_zone_ptr_following(std::numeric_limits<std::uintptr_t>::max() >> ((sizeof(std::uintptr_t) * 8u) - pointerBitCount)),

              // -2
              m_zone_ptr_insert((std::numeric_limits<std::uintptr_t>::max() >> ((sizeof(std::uintptr_t) * 8u) - pointerBitCount)) - 1u)
        {
            assert(pointerBitCount % 8u == 0u);
            assert(insertBlock < static_cast<block_t>(blocks.size()));

            m_insert_block = blocks[insertBlock];
        }

        [[nodiscard]] unsigned GetPointerByteCount() const override
        {
            return m_pointer_byte_count;
        }

        void PushBlock(const block_t block) override
        {
            assert(block < static_cast<block_t>(m_blocks.size()));

            auto* newBlock = m_blocks[block];

            assert(newBlock->m_index == block);

            m_block_stack.push(newBlock);

            if (newBlock->m_type == XBlockType::BLOCK_TYPE_TEMP)
            {
                if (m_temp_sizes.empty())
                    m_temp_sizes.push(0);
                else
                    m_temp_sizes.push(m_temp_sizes.top());
            }
        }

        block_t PopBlock() override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return -1;

            auto* poppedBlock = m_block_stack.top();
            m_block_stack.pop();

            // If temp block is popped, see if its size is bigger than the current maximum temp size
            if (poppedBlock->m_type == XBlockType::BLOCK_TYPE_TEMP)
            {
                const auto tempSize = m_temp_sizes.top();
                m_temp_sizes.pop();

                poppedBlock->m_buffer_size = std::max(tempSize, poppedBlock->m_buffer_size);
            }

            return poppedBlock->m_index;
        }

        void Align(const unsigned align) override
        {
            assert(!m_block_stack.empty());

            if (align > 1)
            {
                auto* block = m_block_stack.top();

                if (block->m_type == XBlockType::BLOCK_TYPE_TEMP)
                    m_temp_sizes.top() = utils::Align(m_temp_sizes.top(), static_cast<size_t>(align));
                else
                    block->m_buffer_size = utils::Align(block->m_buffer_size, static_cast<size_t>(align));
            }
        }

        ZoneOutputOffset WriteDataRaw(const void* src, const size_t size) override
        {
            auto* result = m_zone_data.GetBufferOfSize(size);
            memcpy(result, src, size);

            return ZoneOutputOffset(result);
        }

        ZoneOutputOffset WriteDataInBlock(const void* src, const size_t size) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return ZoneOutputOffset();

            const auto* block = m_block_stack.top();

            void* result = nullptr;
            switch (block->m_type)
            {
            case XBlockType::BLOCK_TYPE_TEMP:
            case XBlockType::BLOCK_TYPE_NORMAL:
                result = m_zone_data.GetBufferOfSize(size);
                memcpy(result, src, size);
                break;

            case XBlockType::BLOCK_TYPE_RUNTIME:
                break;

            case XBlockType::BLOCK_TYPE_DELAY:
                assert(false);
                break;
            }

            IncBlockPos(size);

            return ZoneOutputOffset(result);
        }

        void IncBlockPos(const size_t size) override
        {
            assert(!m_block_stack.empty());

            if (m_block_stack.empty())
                return;

            auto* block = m_block_stack.top();
            if (block->m_type == XBlockType::BLOCK_TYPE_TEMP)
            {
                m_temp_sizes.top() += size;
            }
            else
            {
                block->m_buffer_size += size;
            }
        }

        void WriteNullTerminated(const void* src) override
        {
            const auto len = strlen(static_cast<const char*>(src));
            WriteDataInBlock(src, len + 1);
        }

        ZoneStreamFillWriteAccessor WriteWithFill(const size_t size) override
        {
            // If no block has been pushed, load raw
            if (!m_block_stack.empty())
            {
                const auto* block = m_block_stack.top();

                void* result = nullptr;
                switch (block->m_type)
                {
                case XBlockType::BLOCK_TYPE_TEMP:
                case XBlockType::BLOCK_TYPE_NORMAL:
                    result = m_zone_data.GetBufferOfSize(size);
                    break;

                case XBlockType::BLOCK_TYPE_RUNTIME:
                case XBlockType::BLOCK_TYPE_DELAY:
                    assert(false);
                    break;
                }

                IncBlockPos(size);

                return ZoneStreamFillWriteAccessor(result, size);
            }

            return ZoneStreamFillWriteAccessor(m_zone_data.GetBufferOfSize(size), size);
        }

        void MarkFollowing(const ZoneOutputOffset outputOffset) override
        {
            assert(!m_block_stack.empty());

            auto* ptr = static_cast<char*>(outputOffset.Offset());
            assert(ptr != nullptr);

            if (m_block_stack.top()->m_type == XBlockType::BLOCK_TYPE_TEMP)
            {
                for (auto i = 0u; i < m_pointer_byte_count; i++)
                    ptr[i] = reinterpret_cast<const char*>(&m_zone_ptr_insert)[i];
            }
            else
            {
                for (auto i = 0u; i < m_pointer_byte_count; i++)
                    ptr[i] = reinterpret_cast<const char*>(&m_zone_ptr_following)[i];
            }
        }

        bool ReusableShouldWrite(void* ptr, const ZoneOutputOffset outputOffset, const size_t entrySize, const std::type_index type) override
        {
            assert(!m_block_stack.empty());

            if (ptr == nullptr)
                return false;

            const auto foundEntriesForType = m_reusable_entries.find(type);
            if (foundEntriesForType == m_reusable_entries.end())
            {
                return true;
            }

            for (const auto& entry : foundEntriesForType->second)
            {
                if (ptr >= entry.m_start_ptr && ptr < entry.m_end_ptr)
                {
                    assert((reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(entry.m_start_ptr)) % entrySize == 0);
                    const auto finalZonePointer = entry.m_start_zone_ptr + (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(entry.m_start_ptr));
                    auto* writtenPtrOffset = outputOffset.Offset();

                    for (auto i = 0u; i < m_pointer_byte_count; i++)
                        static_cast<char*>(writtenPtrOffset)[i] = reinterpret_cast<const char*>(&finalZonePointer)[i];

                    return false;
                }
            }

            return true;
        }

        void ReusableAddOffset(void* ptr, size_t size, size_t count, std::type_index type) override
        {
            assert(!m_block_stack.empty());

            const auto inTemp = m_block_stack.top()->m_type == XBlockType::BLOCK_TYPE_TEMP;
            auto zoneOffset = inTemp ? InsertPointer() : GetCurrentZonePointer();
            const auto foundEntriesForType = m_reusable_entries.find(type);
            if (foundEntriesForType == m_reusable_entries.end())
            {
                std::vector<ReusableEntry> entries;
                entries.emplace_back(ptr, size, count, zoneOffset);
                m_reusable_entries.emplace(std::make_pair(type, std::move(entries)));
            }
            else
            {
                foundEntriesForType->second.emplace_back(ptr, size, count, zoneOffset);
            }
        }

    private:
        uintptr_t GetCurrentZonePointer()
        {
            assert(!m_block_stack.empty());
            assert(m_block_stack.top()->m_type == XBlockType::BLOCK_TYPE_NORMAL);

            uintptr_t ptr = 0;
            ptr |= static_cast<uintptr_t>(m_block_stack.top()->m_index) << (sizeof(uintptr_t) * 8 - m_block_bit_count);
            ptr |= m_block_stack.top()->m_buffer_size & (UINTPTR_MAX >> m_block_bit_count);
            ptr++;

            return ptr;
        }

        uintptr_t InsertPointer()
        {
            PushBlock(m_insert_block->m_index);

            Align(sizeof(uintptr_t));
            const auto result = GetCurrentZonePointer();
            IncBlockPos(sizeof(uintptr_t));

            PopBlock();

            return result;
        }

        InMemoryZoneData& m_zone_data;
        std::vector<XBlock*>& m_blocks;

        std::stack<XBlock*> m_block_stack;
        std::stack<size_t> m_temp_sizes;

        unsigned m_block_bit_count;
        unsigned m_pointer_byte_count;
        XBlock* m_insert_block;

        uintptr_t m_zone_ptr_following;
        uintptr_t m_zone_ptr_insert;

        std::unordered_map<std::type_index, std::vector<ReusableEntry>> m_reusable_entries;
    };
} // namespace

ZoneOutputOffset::ZoneOutputOffset()
    : m_offset(nullptr)
{
}

ZoneOutputOffset::ZoneOutputOffset(void* offset)
    : m_offset(offset)
{
}

ZoneOutputOffset ZoneOutputOffset::AtOffset(const size_t innerOffset) const
{
    return ZoneOutputOffset(static_cast<char*>(m_offset) + innerOffset);
}

void ZoneOutputOffset::Inc(const size_t size)
{
    m_offset = static_cast<void*>(static_cast<char*>(m_offset) + size);
}

void* ZoneOutputOffset::Offset() const
{
    return m_offset;
}

std::unique_ptr<ZoneOutputStream>
    ZoneOutputStream::Create(unsigned pointerBitCount, unsigned blockBitCount, std::vector<XBlock*>& blocks, block_t insertBlock, InMemoryZoneData& zoneData)
{
    return std::make_unique<InMemoryZoneOutputStream>(pointerBitCount, blockBitCount, blocks, insertBlock, zoneData);
}

ZoneStreamFillWriteAccessor::ZoneStreamFillWriteAccessor(void* blockBuffer, const size_t bufferSize)
    : m_block_buffer(blockBuffer),
      m_buffer_size(bufferSize)
{
}

ZoneStreamFillWriteAccessor ZoneStreamFillWriteAccessor::AtOffset(const size_t offset) const
{
    return ZoneStreamFillWriteAccessor(static_cast<char*>(m_block_buffer) + offset, m_buffer_size - offset);
}

ZoneOutputOffset ZoneStreamFillWriteAccessor::Offset() const
{
    return ZoneOutputOffset(m_block_buffer);
}
