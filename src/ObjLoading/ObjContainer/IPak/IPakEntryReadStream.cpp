#include "IPakEntryReadStream.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include <cassert>
#include <stdexcept>
#include <minilzo.h>

using namespace ipak_consts;

IPakEntryReadStream::IPakEntryReadStream(IFile* file, IPakStreamManagerActions* streamManagerActions,
                                         uint8_t* chunkBuffer, const int64_t startOffset, const size_t fileSize)
{
    m_file = file;
    m_stream_manager_actions = streamManagerActions;
    m_chunk_buffer = chunkBuffer;

    m_file_offset = 0;
    m_file_head = 0;
    m_file_length = fileSize;

    m_file_buffer = new uint8_t[fileSize];

    m_base_pos = startOffset;
    m_pos = m_base_pos;
    m_buffer_start_pos = 0;
    m_buffer_end_pos = 0;

    lzo_init();
}

IPakEntryReadStream::~IPakEntryReadStream()
{
    Close();
}

bool IPakEntryReadStream::SetChunkBufferWindow(const int64_t startPos, const size_t chunkCount)
{
    // Cannot load more than IPAK_CHUNK_COUNT_PER_READ chunks without overflowing the buffer
    assert(chunkCount <= IPAK_CHUNK_COUNT_PER_READ);

    // The start position must be aligned to IPAK_CHUNK_SIZE
    assert(startPos % IPAK_CHUNK_SIZE == 0);

    const int64_t endPos = startPos + static_cast<int64_t>(chunkCount) * IPAK_CHUNK_SIZE;

    uint8_t* readBuffer = m_chunk_buffer;
    size_t readChunkCount = chunkCount;

    if (startPos >= m_buffer_start_pos && startPos < m_buffer_end_pos)
    {
        if (startPos != m_buffer_start_pos)
        {
            memmove_s(m_chunk_buffer,
                      IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ,
                      &m_chunk_buffer[startPos - m_buffer_start_pos],
                      static_cast<size_t>(m_buffer_end_pos - startPos));
            readBuffer = &m_chunk_buffer[m_buffer_end_pos - startPos];
        }

        readChunkCount = endPos > m_buffer_end_pos
                             ? static_cast<size_t>(endPos - m_buffer_end_pos) / IPAK_CHUNK_SIZE
                             : 0;
    }
    else if (endPos > m_buffer_start_pos && endPos <= m_buffer_end_pos)
    {
        memmove_s(&m_chunk_buffer[m_buffer_start_pos - startPos],
                  IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ - static_cast<size_t>(m_buffer_start_pos - startPos),
                  m_chunk_buffer,
                  static_cast<size_t>(endPos - m_buffer_start_pos));

        readChunkCount = static_cast<size_t>(m_buffer_start_pos - startPos) / IPAK_CHUNK_SIZE;
    }

    if (readChunkCount == 0)
        return true;

    m_stream_manager_actions->StartReading();
    m_file->Goto(startPos);
    const auto readSize = m_file->Read(readBuffer, 1, readChunkCount * IPAK_CHUNK_SIZE);
    m_stream_manager_actions->StopReading();

    m_buffer_start_pos = startPos;
    m_buffer_end_pos = AlignBackwards<int64_t>(m_buffer_start_pos + readSize, IPAK_CHUNK_SIZE);

    return readSize == readChunkCount * IPAK_CHUNK_SIZE;
}

bool IPakEntryReadStream::ValidateBlockHeader(IPakDataBlockHeader* blockHeader) const
{
    if (blockHeader->count > 31)
    {
        printf("IPak block has more than 31 commands: %u -> Invalid\n", blockHeader->count);
        return false;
    }
    if (blockHeader->offset > m_file_length)
    {
        printf("IPak block offset is larger than file itself: %u > %u -> Invalid\n", blockHeader->offset,
               m_file_length);
        return false;
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

    const size_t amountOfReadBlocks = static_cast<size_t>(m_buffer_end_pos - m_buffer_start_pos) / IPAK_CHUNK_SIZE;

    if (requiredChunkCount > amountOfReadBlocks)
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

bool IPakEntryReadStream::ProcessCommand(const size_t commandSize, const bool compressed)
{
    if (compressed)
    {
        lzo_uint outputSize = m_file_length - m_file_head;
        const auto result = lzo1x_decompress(&m_chunk_buffer[m_pos - m_buffer_start_pos], commandSize,
                                             &m_file_buffer[m_file_head], &outputSize, nullptr);

        if (result != LZO_E_OK)
        {
            printf("Decompressing block with lzo failed: %i!\n", result);
            return false;
        }

        m_file_head += outputSize;
    }
    else
    {
        if (m_file_length - m_file_head < commandSize)
        {
            printf(
                "There is not enough space in output buffer to extract data from IPak block: %u required, %u available\n",
                commandSize, m_file_length - m_file_head);
            return false;
        }

        memcpy_s(&m_file_buffer[m_file_head], m_file_length - m_file_head, &m_chunk_buffer[m_pos - m_buffer_start_pos],
                 commandSize);
    }

    return true;
}

bool IPakEntryReadStream::AdvanceStream()
{
    const auto chunkStartPos = AlignBackwards<int64_t>(m_pos, IPAK_CHUNK_SIZE);
    const auto blockOffsetInChunk = static_cast<size_t>(m_pos - chunkStartPos);

    const size_t sizeLeftToRead = m_file_length - m_file_head;
    size_t estimatedChunksToRead = AlignForward(blockOffsetInChunk + sizeLeftToRead, IPAK_CHUNK_SIZE)
        / IPAK_CHUNK_SIZE;

    if (estimatedChunksToRead > IPAK_CHUNK_COUNT_PER_READ)
        estimatedChunksToRead = IPAK_CHUNK_COUNT_PER_READ;

    if (!SetChunkBufferWindow(chunkStartPos, estimatedChunksToRead))
        return false;

    const auto blockHeader = reinterpret_cast<IPakDataBlockHeader*>(&m_chunk_buffer[blockOffsetInChunk]);

    if (!ValidateBlockHeader(blockHeader))
        return false;

    if (!AdjustChunkBufferWindowForBlockHeader(blockHeader, blockOffsetInChunk))
        return false;

    m_pos += sizeof IPakDataBlockHeader;

    for (unsigned commandIndex = 0; commandIndex < blockHeader->count; commandIndex++)
    {
        if (!ProcessCommand(blockHeader->_commands[commandIndex].size,
                            blockHeader->_commands[commandIndex].compressed == 1))
            return false;
    }

    m_pos = AlignForward<int64_t>(m_pos, sizeof IPakDataBlockHeader);

    return true;
}

bool IPakEntryReadStream::IsOpen()
{
    return m_file != nullptr;
}

size_t IPakEntryReadStream::Read(void* buffer, const size_t elementSize, const size_t elementCount)
{
    const size_t bufferSize = elementCount * elementSize;
    size_t sizeToRead = bufferSize <= m_file_length - m_file_offset ? bufferSize : m_file_length - m_file_offset;

    if (sizeToRead)
        sizeToRead = m_file_length - m_file_offset;

    while (m_file_offset + sizeToRead > m_file_head)
    {
        if (!AdvanceStream())
        {
            if (m_file_head - m_file_offset < sizeToRead)
                sizeToRead = m_file_head - m_file_offset;
        }
    }

    if (sizeToRead > 0)
    {
        memcpy_s(buffer, bufferSize, &m_file_buffer[m_file_offset], sizeToRead);
    }

    return sizeToRead;
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
        m_file_offset += static_cast<size_t>(amount);

        if (m_file_offset > m_file_length)
            m_file_offset = m_file_length;
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
    if (pos <= 0)
    {
        m_file_offset = static_cast<size_t>(pos);

        if (m_file_offset > m_file_length)
            m_file_offset = m_file_length;
    }
}

void IPakEntryReadStream::GotoEnd()
{
    m_pos = m_file_length;
}

void IPakEntryReadStream::Close()
{
    if (IsOpen())
    {
        m_file = nullptr;

        delete[] m_file_buffer;
        m_file_buffer = nullptr;

        m_stream_manager_actions->CloseStream(this);
    }
}
