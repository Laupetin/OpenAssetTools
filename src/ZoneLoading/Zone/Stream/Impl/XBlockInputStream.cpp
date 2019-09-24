#include "XBlockInputStream.h"
#include "Loading/Exception/BlockOverflowException.h"
#include "Loading/Exception/InvalidOffsetBlockException.h"
#include "Loading/Exception/InvalidOffsetBlockOffsetException.h"

#include <cassert>

XBlockInputStream::XBlockInputStream(std::vector<XBlock*>& blocks, ILoadingStream* stream, const int blockBitCount, const block_t insertBlock) : m_blocks(blocks)
{
    m_stream = stream;

    const unsigned int blockCount = blocks.size();
    m_block_offsets = new size_t[blockCount]{0};
    m_block_in_stack = new unsigned int[blockCount]{0};

    m_block_bit_count = blockBitCount;

    assert(insertBlock >= 0 && insertBlock < static_cast<block_t>(blocks.size()));
    m_insert_block = blocks[insertBlock];
}

XBlockInputStream::~XBlockInputStream()
{
    delete[] m_block_offsets;
    m_block_offsets = nullptr;

    delete[] m_block_in_stack;
    m_block_in_stack = nullptr;

    assert(m_block_stack.empty());
}

void XBlockInputStream::Align(const int align)
{
    assert(!m_block_stack.empty());

    if(align > 0)
    {
        const block_t blockIndex = m_block_stack.top()->m_index;
        m_block_offsets[blockIndex] = (m_block_offsets[blockIndex] + align - 1) / align * align;
    }
}

void XBlockInputStream::PushBlock(const block_t block)
{
    assert(block >= 0 && block < static_cast<block_t>(m_blocks.size()));

    XBlock* newBlock = m_blocks[block];

    assert(newBlock->m_index == block);

    m_block_stack.push(newBlock);
    m_block_in_stack[newBlock->m_index]++;
}

block_t XBlockInputStream::PopBlock()
{
    assert(!m_block_stack.empty());

    if(m_block_stack.empty())
        return -1;

    const XBlock* poppedBlock = m_block_stack.top();

    m_block_stack.pop();
    m_block_in_stack[poppedBlock->m_index]--;

    // If the temp block is not used anymore right now, reset it to the buffer start since as the name suggests, the data inside is temporary.
    if(poppedBlock->m_type == XBlock::BLOCK_TYPE_TEMP && m_block_in_stack[poppedBlock->m_index] == 0)
    {
        m_block_offsets[poppedBlock->m_index] = 0;
    }

    return poppedBlock->m_index;
}

void* XBlockInputStream::Alloc(const int align)
{
    assert(!m_block_stack.empty());

    if(m_block_stack.empty())
        return nullptr;

    XBlock* block = m_block_stack.top();

    Align(align);

    if(m_block_offsets[block->m_index] >= block->m_buffer_size)
    {
        throw BlockOverflowException(block);
    }

    return &block->m_buffer[m_block_offsets[block->m_index]];
}

void XBlockInputStream::LoadData(const size_t size)
{
    assert(!m_block_stack.empty());

    if(m_block_stack.empty())
        return;

    XBlock* block = m_block_stack.top();

    if(m_block_offsets[block->m_index] + size >= block->m_buffer_size)
    {
        throw BlockOverflowException(block);
    }

    m_stream->Load(&block->m_buffer[m_block_offsets[block->m_index]], size);

    m_block_offsets[block->m_index] += size;
}

void XBlockInputStream::LoadNullTerminated()
{
    assert(!m_block_stack.empty());

    if(m_block_stack.empty())
        return;

    XBlock* block = m_block_stack.top();

    uint8_t byte;
    size_t offset = m_block_offsets[block->m_index];
    do
    {
        m_stream->Load(&byte, 1);
        block->m_buffer[offset++] = byte;

        if(offset >= block->m_buffer_size)
        {
            throw BlockOverflowException(block);
        }

    } while(byte != 0);

    m_block_offsets[block->m_index] = offset;
}

void** XBlockInputStream::InsertPointer()
{
    m_block_stack.push(m_insert_block);

    Align(sizeof(void*));
    
    if(m_block_offsets[m_insert_block->m_index] + sizeof(void*) >= m_insert_block->m_buffer_size)
    {
        throw BlockOverflowException(m_insert_block);
    }

    void** ptr = reinterpret_cast<void**>(&m_insert_block->m_buffer[m_block_offsets[m_insert_block->m_index]]);

    m_block_stack.pop();

    return ptr;
}

void* XBlockInputStream::ConvertOffsetToPointer(const void* offset)
{
    const block_t blockNum = reinterpret_cast<uintptr_t>(offset) >> (sizeof(offset) * 8 - m_block_bit_count);
    const size_t blockOffset = reinterpret_cast<uintptr_t>(offset) & (UINTPTR_MAX >> m_block_bit_count);

    if(blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
    {
        throw InvalidOffsetBlockException(blockNum);
    }

    XBlock* block = m_blocks[blockNum];

    if(block->m_buffer_size <= blockOffset)
    {
        throw InvalidOffsetBlockOffsetException(block, blockOffset);
    }

    return &block->m_buffer[blockOffset];
}

void* XBlockInputStream::ConvertOffsetToAlias(const void* offset)
{
    const block_t blockNum = reinterpret_cast<uintptr_t>(offset) >> (sizeof(offset) * 8 - m_block_bit_count);
    const size_t blockOffset = reinterpret_cast<uintptr_t>(offset) & (UINTPTR_MAX >> m_block_bit_count);

    if(blockNum < 0 || blockNum >= static_cast<block_t>(m_blocks.size()))
    {
        throw InvalidOffsetBlockException(blockNum);
    }

    XBlock* block = m_blocks[blockNum];

    if(block->m_buffer_size <= blockOffset + sizeof(void*))
    {
        throw InvalidOffsetBlockOffsetException(block, blockOffset);
    }

    return *reinterpret_cast<void**>(&block->m_buffer[blockOffset]);
}