#include "InMemoryZoneOutputStream.h"

#include <cassert>
#include <cstring>

InMemoryZoneOutputStream::InMemoryZoneOutputStream(InMemoryZoneData* zoneData, std::vector<XBlock*> blocks, const int blockBitCount, const block_t insertBlock)
    : m_zone_data(zoneData),
      m_blocks(std::move(blocks)),
      m_block_bit_count(blockBitCount),
      m_insert_block(m_blocks[insertBlock])
{
}

InMemoryZoneOutputStream::ReusableEntry::ReusableEntry(void* startPtr, const size_t entrySize, const size_t entryCount, const uintptr_t startZonePtr)
    : m_start_ptr(startPtr),
      m_end_ptr(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(startPtr) + entrySize * entryCount)),
      m_start_zone_ptr(startZonePtr),
      m_entry_size(entrySize),
      m_entry_count(entryCount)
{
}

void InMemoryZoneOutputStream::PushBlock(const block_t block)
{
    assert(block >= 0 && block < static_cast<block_t>(m_blocks.size()));

    auto* newBlock = m_blocks[block];

    assert(newBlock->m_index == block);

    m_block_stack.push(newBlock);

    if (newBlock->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
    {
        if (m_temp_sizes.empty())
            m_temp_sizes.push(0);
        else
            m_temp_sizes.push(m_temp_sizes.top());
    }
}

block_t InMemoryZoneOutputStream::PopBlock()
{
    assert(!m_block_stack.empty());

    if (m_block_stack.empty())
        return -1;

    auto* poppedBlock = m_block_stack.top();
    m_block_stack.pop();

    // If temp block is popped, see if its size is bigger than the current maximum temp size
    if (poppedBlock->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
    {
        const auto tempSize = m_temp_sizes.top();
        m_temp_sizes.pop();

        if (tempSize > poppedBlock->m_buffer_size)
            poppedBlock->m_buffer_size = tempSize;
    }

    return poppedBlock->m_index;
}

void InMemoryZoneOutputStream::Align(const int align)
{
    assert(!m_block_stack.empty());

    if (align > 1)
    {
        auto* block = m_block_stack.top();

        if (block->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
            m_temp_sizes.top() = (m_temp_sizes.top() + align - 1) / align * align;
        else
            block->m_buffer_size = (block->m_buffer_size + align - 1) / align * align;
    }
}

void* InMemoryZoneOutputStream::WriteDataRaw(const void* src, const size_t size)
{
    auto* result = m_zone_data->GetBufferOfSize(size);
    memcpy(result, src, size);
    return result;
}

void* InMemoryZoneOutputStream::WriteDataInBlock(const void* src, const size_t size)
{
    assert(!m_block_stack.empty());

    if (m_block_stack.empty())
        return nullptr;

    auto* block = m_block_stack.top();

    void* result = nullptr;
    switch (block->m_type)
    {
    case XBlock::Type::BLOCK_TYPE_TEMP:
    case XBlock::Type::BLOCK_TYPE_NORMAL:
        result = m_zone_data->GetBufferOfSize(size);
        memcpy(result, src, size);
        break;

    case XBlock::Type::BLOCK_TYPE_RUNTIME:
        break;

    case XBlock::Type::BLOCK_TYPE_DELAY:
        assert(false);
        break;
    }

    IncBlockPos(size);
    return result;
}

void InMemoryZoneOutputStream::IncBlockPos(const size_t size)
{
    assert(!m_block_stack.empty());

    if (m_block_stack.empty())
        return;

    auto* block = m_block_stack.top();
    if (block->m_type == XBlock::Type::BLOCK_TYPE_TEMP)
    {
        m_temp_sizes.top() += size;
    }
    else
    {
        block->m_buffer_size += size;
    }
}

void InMemoryZoneOutputStream::WriteNullTerminated(const void* src)
{
    const auto len = strlen(static_cast<const char*>(src));
    WriteDataInBlock(src, len + 1);
}

uintptr_t InMemoryZoneOutputStream::GetCurrentZonePointer()
{
    assert(!m_block_stack.empty());
    assert(m_block_stack.top()->m_type == XBlock::Type::BLOCK_TYPE_NORMAL);

    uintptr_t ptr = 0;
    ptr |= static_cast<uintptr_t>(m_block_stack.top()->m_index) << (sizeof(uintptr_t) * 8 - m_block_bit_count);
    ptr |= m_block_stack.top()->m_buffer_size & (UINTPTR_MAX >> m_block_bit_count);
    ptr++;

    return ptr;
}

uintptr_t InMemoryZoneOutputStream::InsertPointer()
{
    PushBlock(m_insert_block->m_index);

    Align(sizeof(uintptr_t));
    const auto result = GetCurrentZonePointer();
    IncBlockPos(sizeof(uintptr_t));

    PopBlock();

    return result;
}

void InMemoryZoneOutputStream::MarkFollowing(void** pPtr)
{
    assert(!m_block_stack.empty());
    assert(pPtr != nullptr);
    *pPtr = m_block_stack.top()->m_type == XBlock::Type::BLOCK_TYPE_TEMP ? PTR_INSERT : PTR_FOLLOWING;
}

bool InMemoryZoneOutputStream::ReusableShouldWrite(void** pPtr, const size_t entrySize, const std::type_index type)
{
    assert(!m_block_stack.empty());
    assert(pPtr != nullptr);

    if (*pPtr == nullptr)
        return false;

    const auto foundEntriesForType = m_reusable_entries.find(type);
    if (foundEntriesForType == m_reusable_entries.end())
    {
        return true;
    }

    for (const auto& entry : foundEntriesForType->second)
    {
        if (*pPtr >= entry.m_start_ptr && *pPtr < entry.m_end_ptr)
        {
            assert((reinterpret_cast<uintptr_t>(*pPtr) - reinterpret_cast<uintptr_t>(entry.m_start_ptr)) % entrySize == 0);
            *pPtr = reinterpret_cast<void*>(entry.m_start_zone_ptr + (reinterpret_cast<uintptr_t>(*pPtr) - reinterpret_cast<uintptr_t>(entry.m_start_ptr)));
            return false;
        }
    }

    return true;
}

void InMemoryZoneOutputStream::ReusableAddOffset(void* ptr, size_t size, size_t count, std::type_index type)
{
    assert(!m_block_stack.empty());

    const auto inTemp = m_block_stack.top()->m_type == XBlock::Type::BLOCK_TYPE_TEMP;
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
