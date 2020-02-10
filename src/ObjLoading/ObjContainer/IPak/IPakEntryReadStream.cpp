#include "IPakEntryReadStream.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include <cassert>
#include <stdexcept>
#include <minilzo.h>

using namespace ipak_consts;

IPakEntryReadStream::IPakEntryReadStream(IFile* file, IPakStreamManagerActions* streamManagerActions,
                                         uint8_t* chunkBuffer, const int64_t startOffset, const size_t entrySize)
    : m_decompress_buffer{}
{
    m_file = file;
    m_stream_manager_actions = streamManagerActions;
    m_chunk_buffer = chunkBuffer;

    m_file_offset = 0;
    m_file_head = 0;
    m_entry_size = entrySize;

    m_base_pos = startOffset;
    m_end_pos = startOffset + entrySize;
    m_pos = m_base_pos;
    m_buffer_start_pos = 0;
    m_buffer_end_pos = 0;

    m_current_block = nullptr;
    m_next_command = 0;
    m_current_command_buffer = nullptr;
    m_current_command_length = 0;
    m_current_command_offset = 0;

    lzo_init();
}

IPakEntryReadStream::~IPakEntryReadStream()
{
    Close();
}

size_t IPakEntryReadStream::ReadChunks(uint8_t* buffer, const int64_t startPos, const size_t chunkCount) const
{
    m_stream_manager_actions->StartReading();
    m_file->Goto(startPos);
    const auto readSize = m_file->Read(buffer, 1, chunkCount * IPAK_CHUNK_SIZE);
    m_stream_manager_actions->StopReading();

    return readSize / IPAK_CHUNK_SIZE;
}

bool IPakEntryReadStream::SetChunkBufferWindow(const int64_t startPos, size_t chunkCount)
{
    // Cannot load more than IPAK_CHUNK_COUNT_PER_READ chunks without overflowing the buffer
    assert(chunkCount <= IPAK_CHUNK_COUNT_PER_READ);

    if (chunkCount > IPAK_CHUNK_COUNT_PER_READ)
        chunkCount = IPAK_CHUNK_COUNT_PER_READ;

    // The start position must be aligned to IPAK_CHUNK_SIZE
    assert(startPos % IPAK_CHUNK_SIZE == 0);

    if (chunkCount == 0)
    {
        m_buffer_start_pos = startPos;
        m_buffer_end_pos = startPos;
        return true;
    }

    const int64_t endPos = startPos + static_cast<int64_t>(chunkCount) * IPAK_CHUNK_SIZE;

    if (startPos >= m_buffer_start_pos && startPos < m_buffer_end_pos)
    {
        if (m_buffer_start_pos != startPos)
        {
            const int64_t moveEnd = endPos < m_buffer_end_pos ? endPos : m_buffer_end_pos;
            memmove_s(m_chunk_buffer,
                      IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ,
                      &m_chunk_buffer[startPos - m_buffer_start_pos],
                      static_cast<size_t>(moveEnd - startPos));
            m_buffer_start_pos = startPos;
        }

        if (endPos > m_buffer_end_pos)
        {
            const size_t readChunkCount = ReadChunks(&m_chunk_buffer[m_buffer_end_pos - startPos],
                                                     m_buffer_end_pos,
                                                     static_cast<size_t>(endPos - m_buffer_end_pos) / IPAK_CHUNK_SIZE);

            m_buffer_end_pos += static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

            return m_buffer_end_pos == endPos;
        }
        else
        {
            m_buffer_end_pos = endPos;

            return true;
        }
    }
    else if (endPos > m_buffer_start_pos && endPos <= m_buffer_end_pos)
    {
        memmove_s(&m_chunk_buffer[m_buffer_start_pos - startPos],
                  IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ - static_cast<size_t>(m_buffer_start_pos - startPos),
                  m_chunk_buffer,
                  static_cast<size_t>(endPos - m_buffer_start_pos));

        const size_t readChunkCount = ReadChunks(m_chunk_buffer,
                                                 startPos,
                                                 static_cast<size_t>(m_buffer_start_pos - startPos) / IPAK_CHUNK_SIZE);

        m_buffer_start_pos = startPos;
        m_buffer_end_pos = readChunkCount == (m_buffer_start_pos - startPos) / IPAK_CHUNK_SIZE
                               ? endPos
                               : startPos + static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

        return m_buffer_end_pos == endPos;
    }

    const size_t readChunkCount = ReadChunks(m_chunk_buffer,
                                             startPos,
                                             chunkCount);

    m_buffer_start_pos = startPos;
    m_buffer_end_pos = startPos + static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

    return chunkCount == readChunkCount;
}

bool IPakEntryReadStream::ValidateBlockHeader(IPakDataBlockHeader* blockHeader) const
{
    if (blockHeader->count > 31)
    {
        printf("IPak block has more than 31 commands: %u -> Invalid\n", blockHeader->count);
        return false;
    }
    if (blockHeader->offset != m_file_head)
    {
        // A matching offset is only relevant if a command contains data.
        for(unsigned currentCommand = 0; currentCommand < blockHeader->count; currentCommand++)
        {
            if(blockHeader->_commands[currentCommand].compressed == 0
                || blockHeader->_commands[currentCommand].compressed == 1)
            {
                printf("IPak block offset is not the file head: %u != %u -> Invalid\n", blockHeader->offset, m_file_head);
                return false;
            }
        }
    }

    return true;
}

bool IPakEntryReadStream::AdjustChunkBufferWindowForBlockHeader(IPakDataBlockHeader* blockHeader,
                                                                const size_t blockOffsetInChunk)
{
    size_t commandsSize = 0;
    for (unsigned commandIndex = 0; commandIndex < blockHeader->count; commandIndex++)
    {
        commandsSize += blockHeader->_commands[commandIndex].size;
    }

    const size_t requiredChunkCount = AlignForward<size_t>(
        blockOffsetInChunk + sizeof IPakDataBlockHeader + commandsSize, IPAK_CHUNK_SIZE) / IPAK_CHUNK_SIZE;

    const size_t amountOfReadChunks = static_cast<size_t>(m_buffer_end_pos - m_buffer_start_pos) / IPAK_CHUNK_SIZE;

    if (requiredChunkCount > amountOfReadChunks)
    {
        if (requiredChunkCount > IPAK_CHUNK_COUNT_PER_READ)
        {
            printf("IPak block spans over more than %u blocks (%u), which is not supported.\n",
                   IPAK_CHUNK_COUNT_PER_READ, requiredChunkCount);
            return false;
        }

        if (!SetChunkBufferWindow(m_buffer_start_pos, requiredChunkCount))
            return false;
    }

    return true;
}

bool IPakEntryReadStream::NextBlock()
{
    if (m_pos >= m_end_pos)
        return false;

    m_pos = AlignForward<int64_t>(m_pos, sizeof IPakDataBlockHeader);

    const auto chunkStartPos = AlignBackwards<int64_t>(m_pos, IPAK_CHUNK_SIZE);
    const auto blockOffsetInChunk = static_cast<size_t>(m_pos - chunkStartPos);

    const size_t sizeLeftToRead = m_entry_size - m_file_head;
    size_t estimatedChunksToRead = AlignForward(m_entry_size - static_cast<size_t>(m_pos - m_base_pos), IPAK_CHUNK_SIZE)
        / IPAK_CHUNK_SIZE;

    if (estimatedChunksToRead > IPAK_CHUNK_COUNT_PER_READ)
        estimatedChunksToRead = IPAK_CHUNK_COUNT_PER_READ;

    if (!SetChunkBufferWindow(chunkStartPos, estimatedChunksToRead))
        return false;

    m_current_block = reinterpret_cast<IPakDataBlockHeader*>(&m_chunk_buffer[blockOffsetInChunk]);

    if (!ValidateBlockHeader(m_current_block))
        return false;

    if (!AdjustChunkBufferWindowForBlockHeader(m_current_block, blockOffsetInChunk))
        return false;

    m_pos += sizeof IPakDataBlockHeader;
    m_next_command = 0;

    return true;
}

bool IPakEntryReadStream::ProcessCommand(const size_t commandSize, const int compressed)
{
    if (compressed > 0)
    {
        if (compressed == 1)
        {
            lzo_uint outputSize = sizeof m_decompress_buffer;
            const auto result = lzo1x_decompress_safe(&m_chunk_buffer[m_pos - m_buffer_start_pos], commandSize,
                                                      m_decompress_buffer, &outputSize, nullptr);

            if (result != LZO_E_OK)
            {
                printf("Decompressing block with lzo failed: %i!\n", result);
                return false;
            }

            m_current_command_buffer = m_decompress_buffer;
            m_current_command_length = outputSize;
            m_current_command_offset = 0;
            m_file_head += outputSize;
        }
    }
    else
    {
        m_current_command_buffer = &m_chunk_buffer[m_pos - m_buffer_start_pos];
        m_current_command_length = commandSize;
        m_current_command_offset = 0;
        m_file_head += commandSize;
    }
    m_pos += commandSize;

    return true;
}

bool IPakEntryReadStream::AdvanceStream()
{
    if (m_current_block == nullptr || m_next_command >= m_current_block->count)
    {
        if (!NextBlock())
            return false;
    }

    ProcessCommand(m_current_block->_commands[m_next_command].size,
                   m_current_block->_commands[m_next_command].compressed);
    m_next_command++;

    return true;
}

bool IPakEntryReadStream::IsOpen()
{
    return m_file != nullptr;
}

size_t IPakEntryReadStream::Read(void* buffer, const size_t elementSize, const size_t elementCount)
{
    auto* destBuffer = static_cast<uint8_t*>(buffer);
    const size_t bufferSize = elementCount * elementSize;
    size_t countRead = 0;

    while (countRead < bufferSize)
    {
        if (m_current_command_offset >= m_current_command_length)
        {
            if (!AdvanceStream())
                break;
        }

        size_t sizeToRead = bufferSize - countRead;
        if (sizeToRead > m_current_command_length - m_current_command_offset)
            sizeToRead = m_current_command_length - m_current_command_offset;

        if (sizeToRead > 0)
        {
            memcpy_s(&destBuffer[countRead], bufferSize - countRead,
                     &m_current_command_buffer[m_current_command_offset], sizeToRead);
            countRead += sizeToRead;
            m_current_command_offset += sizeToRead;
            m_file_offset += sizeToRead;
        }
    }

    return countRead / elementSize;
}

size_t IPakEntryReadStream::Write(const void* data, size_t elementSize, size_t elementCount)
{
    // This is not meant for writing.
    assert(false);
    throw std::runtime_error("This is not a stream for output!");
}

void IPakEntryReadStream::Skip(const int64_t amount)
{
    if (amount > 0)
    {
        const size_t targetOffset = m_file_offset + static_cast<size_t>(amount);

        while (m_file_head < targetOffset)
        {
            if (!AdvanceStream())
                break;
        }

        if (targetOffset <= m_file_head)
        {
            m_current_command_offset = m_current_command_length - (m_file_head - targetOffset);
            m_file_offset = targetOffset;
        }
        else
        {
            m_current_command_offset = m_current_command_length;
            m_file_offset = m_file_head;
        }
    }
}

size_t IPakEntryReadStream::Printf(const char* fmt, ...)
{
    // This is not meant for writing.
    assert(false);
    throw std::runtime_error("This is not a stream for output!");
}

int64_t IPakEntryReadStream::Pos()
{
    return m_file_offset;
}

void IPakEntryReadStream::Goto(const int64_t pos)
{
    if (pos == 0)
    {
        m_pos = m_base_pos;

        m_file_head = 0;
        m_file_offset = 0;

        m_current_block = nullptr;
        m_next_command = 0;
        m_current_command_buffer = nullptr;
        m_current_command_length = 0;
        m_current_command_offset = 0;
    }
    else if (pos > m_file_offset)
    {
        Skip(pos - m_file_offset);
    }
    else
    {
        // Not implemented due to being too time consuming.
        // Can be added if necessary.
        assert(false);
        throw std::runtime_error("Operation not supported!");
    }
}

void IPakEntryReadStream::GotoEnd()
{
    // Not implemented due to being too time consuming.
    // Can be added if necessary.
    assert(false);
    throw std::runtime_error("Operation not supported!");
}

void IPakEntryReadStream::Close()
{
    if (IsOpen())
    {
        m_file = nullptr;

        m_stream_manager_actions->CloseStream(this);
    }
}
