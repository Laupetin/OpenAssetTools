#include "IPakEntryReadStream.h"

#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <minilzo.h>

using namespace ipak_consts;

IPakEntryReadStream::IPakEntryReadStream(
    std::istream& stream, IPakStreamManagerActions* streamManagerActions, uint8_t* chunkBuffer, const int64_t startOffset, const size_t entrySize)
    : m_chunk_buffer(chunkBuffer),
      m_stream(stream),
      m_stream_manager_actions(streamManagerActions),
      m_file_offset(0),
      m_file_head(0),
      m_entry_size(entrySize),
      m_decompress_buffer{},
      m_current_block(nullptr),
      m_next_command(0),
      m_current_command_buffer(nullptr),
      m_current_command_length(0),
      m_current_command_offset(0),
      m_pos(startOffset),
      m_base_pos(startOffset),
      m_end_pos(startOffset + static_cast<int64_t>(entrySize)),
      m_buffer_start_pos(0),
      m_buffer_end_pos(0)
{
    lzo_init();
}

IPakEntryReadStream::~IPakEntryReadStream()
{
    close();
}

size_t IPakEntryReadStream::ReadChunks(uint8_t* buffer, const int64_t startPos, const size_t chunkCount) const
{
    m_stream_manager_actions->StartReading();

    m_stream.seekg(startPos);
    m_stream.read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(chunkCount) * IPAK_CHUNK_SIZE);
    const auto readSize = static_cast<size_t>(m_stream.gcount());

    m_stream_manager_actions->StopReading();

    return readSize / IPAK_CHUNK_SIZE;
}

bool IPakEntryReadStream::SetChunkBufferWindow(const int64_t startPos, size_t chunkCount)
{
    // Cannot load more than IPAK_CHUNK_COUNT_PER_READ chunks without overflowing the buffer
    assert(chunkCount <= IPAK_CHUNK_COUNT_PER_READ);
    chunkCount = std::min(chunkCount, IPAK_CHUNK_COUNT_PER_READ);

    // The start position must be aligned to IPAK_CHUNK_SIZE
    assert(startPos % IPAK_CHUNK_SIZE == 0);

    if (chunkCount == 0)
    {
        m_buffer_start_pos = startPos;
        m_buffer_end_pos = startPos;
        return true;
    }

    const auto endPos = startPos + static_cast<int64_t>(chunkCount) * static_cast<int64_t>(IPAK_CHUNK_SIZE);

    // Check whether the start position is already part of the loaded data
    // We might be able to reuse previously loaded data
    if (startPos >= m_buffer_start_pos && startPos < m_buffer_end_pos)
    {
        // Check whether we need to move data from inside the buffer to the start to account for new start
        if (m_buffer_start_pos != startPos)
        {
            const auto moveEnd = endPos < m_buffer_end_pos ? endPos : m_buffer_end_pos;
            assert(IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ >= static_cast<size_t>(moveEnd - startPos));
            memmove(m_chunk_buffer, &m_chunk_buffer[startPos - m_buffer_start_pos], static_cast<size_t>(moveEnd - startPos));
            m_buffer_start_pos = startPos;
        }

        // Check whether we need to load additional data that was not previously loaded
        if (endPos > m_buffer_end_pos)
        {
            const auto readChunkCount =
                ReadChunks(&m_chunk_buffer[m_buffer_end_pos - startPos], m_buffer_end_pos, static_cast<size_t>(endPos - m_buffer_end_pos) / IPAK_CHUNK_SIZE);

            m_buffer_end_pos += static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

            return m_buffer_end_pos == endPos;
        }

        m_buffer_end_pos = endPos;
        return true;
    }

    // Check whether the end position is already part of the loaded data
    if (endPos > m_buffer_start_pos && endPos <= m_buffer_end_pos)
    {
        assert(IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ - static_cast<size_t>(m_buffer_start_pos - startPos)
               >= static_cast<size_t>(endPos - m_buffer_start_pos));

        // Move data to make sure the end is at the appropriate position to be able to load the missing data in the front
        memmove(&m_chunk_buffer[m_buffer_start_pos - startPos], m_chunk_buffer, static_cast<size_t>(endPos - m_buffer_start_pos));

        // We already established that the start of the buffer is not already loaded so we will need to load additional data nonetheless
        const auto readChunkCount = ReadChunks(m_chunk_buffer, startPos, static_cast<size_t>(m_buffer_start_pos - startPos) / IPAK_CHUNK_SIZE);

        m_buffer_start_pos = startPos;
        m_buffer_end_pos =
            readChunkCount == (m_buffer_start_pos - startPos) / IPAK_CHUNK_SIZE ? endPos : startPos + static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

        return m_buffer_end_pos == endPos;
    }

    // None of the data needed is already loaded -> Load everything and do not reuse any previously loaded data
    const auto readChunkCount = ReadChunks(m_chunk_buffer, startPos, chunkCount);

    m_buffer_start_pos = startPos;
    m_buffer_end_pos = startPos + static_cast<int64_t>(readChunkCount) * IPAK_CHUNK_SIZE;

    return chunkCount == readChunkCount;
}

bool IPakEntryReadStream::ValidateBlockHeader(const IPakDataBlockHeader* blockHeader) const
{
    if (blockHeader->countAndOffset.count > 31)
    {
        con::error("IPak block has more than 31 commands: {} -> Invalid", blockHeader->countAndOffset.count);
        return false;
    }

    // We expect the current file to be continued where we left off
    if (static_cast<int64_t>(blockHeader->countAndOffset.offset) != m_file_head)
    {
        // A matching offset is only relevant if a command contains data
        for (unsigned currentCommand = 0; currentCommand < blockHeader->countAndOffset.count; currentCommand++)
        {
            // If compressed is not 0 or 1 it will not be read and therefore it is okay when the offset does not match
            // The game uses IPAK_COMMAND_SKIP as value for compressed when it intends to skip the specified amount of data
            if (blockHeader->commands[currentCommand].compressed == 0 || blockHeader->commands[currentCommand].compressed == 1)
            {
                con::error("IPak block offset ({}) is not the file head ({}) -> Invalid", blockHeader->countAndOffset.offset, m_file_head);
                return false;
            }
        }
    }

    return true;
}

bool IPakEntryReadStream::AdjustChunkBufferWindowForBlockHeader(const IPakDataBlockHeader* blockHeader, const size_t blockOffsetInChunk)
{
    size_t commandsSize = 0;
    for (unsigned commandIndex = 0; commandIndex < blockHeader->countAndOffset.count; commandIndex++)
    {
        commandsSize += blockHeader->commands[commandIndex].size;
    }

    const size_t requiredChunkCount = AlignForward<size_t>(blockOffsetInChunk + sizeof(IPakDataBlockHeader) + commandsSize, IPAK_CHUNK_SIZE) / IPAK_CHUNK_SIZE;

    const size_t amountOfReadChunks = static_cast<size_t>(m_buffer_end_pos - m_buffer_start_pos) / IPAK_CHUNK_SIZE;

    if (requiredChunkCount > amountOfReadChunks)
    {
        if (requiredChunkCount > IPAK_CHUNK_COUNT_PER_READ)
        {
            con::error("IPak block spans over more than {} chunks ({}), which is not supported.", IPAK_CHUNK_COUNT_PER_READ, requiredChunkCount);
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

    m_pos = AlignForward<int64_t>(m_pos, sizeof(IPakDataBlockHeader));

    const auto chunkStartPos = AlignBackwards<int64_t>(m_pos, IPAK_CHUNK_SIZE);
    const auto blockOffsetInChunk = static_cast<size_t>(m_pos - chunkStartPos);

    auto estimatedChunksToRead = AlignForward(m_entry_size - static_cast<size_t>(m_pos - m_base_pos), IPAK_CHUNK_SIZE) / IPAK_CHUNK_SIZE;

    if (estimatedChunksToRead > IPAK_CHUNK_COUNT_PER_READ)
        estimatedChunksToRead = IPAK_CHUNK_COUNT_PER_READ;

    if (!SetChunkBufferWindow(chunkStartPos, estimatedChunksToRead))
        return false;

    m_current_block = reinterpret_cast<IPakDataBlockHeader*>(&m_chunk_buffer[blockOffsetInChunk]);

    if (!ValidateBlockHeader(m_current_block))
        return false;

    if (!AdjustChunkBufferWindowForBlockHeader(m_current_block, blockOffsetInChunk))
        return false;

    m_pos += sizeof(IPakDataBlockHeader);
    m_next_command = 0;

    return true;
}

bool IPakEntryReadStream::ProcessCommand(const size_t commandSize, const int compressed)
{
    if (compressed > 0)
    {
        if (compressed == 1)
        {
            lzo_uint outputSize = sizeof(m_decompress_buffer);
            const auto result = lzo1x_decompress_safe(&m_chunk_buffer[m_pos - m_buffer_start_pos], commandSize, m_decompress_buffer, &outputSize, nullptr);

            if (result != LZO_E_OK)
            {
                con::error("Decompressing block with lzo failed: {}!", result);
                return false;
            }

            m_current_command_buffer = m_decompress_buffer;
            m_current_command_length = outputSize;
            m_current_command_offset = 0;
            m_file_head += static_cast<int64_t>(outputSize);
        }
        else
        {
            // Do not process data but instead skip specified commandSize
        }
    }
    else
    {
        m_current_command_buffer = &m_chunk_buffer[m_pos - m_buffer_start_pos];
        m_current_command_length = commandSize;
        m_current_command_offset = 0;
        m_file_head += static_cast<int64_t>(commandSize);
    }
    m_pos += static_cast<int64_t>(commandSize);

    return true;
}

bool IPakEntryReadStream::AdvanceStream()
{
    if (m_current_block == nullptr || m_next_command >= m_current_block->countAndOffset.count)
    {
        if (!NextBlock())
            return false;
    }

    ProcessCommand(m_current_block->commands[m_next_command].size, m_current_block->commands[m_next_command].compressed);
    m_next_command++;

    return true;
}

bool IPakEntryReadStream::is_open() const
{
    return !m_stream.eof();
}

bool IPakEntryReadStream::close()
{
    if (is_open())
    {
        m_stream_manager_actions->CloseStream(this);
    }

    return true;
}

std::streamsize IPakEntryReadStream::showmanyc()
{
    return m_end_pos - m_pos;
}

std::streambuf::int_type IPakEntryReadStream::underflow()
{
    while (true)
    {
        if (m_current_command_offset >= m_current_command_length)
        {
            if (!AdvanceStream())
                break;
        }

        if (m_current_command_length - m_current_command_offset < 1)
            continue;

        return m_current_command_buffer[m_current_command_offset];
    }

    return EOF;
}

std::streambuf::int_type IPakEntryReadStream::uflow()
{
    while (true)
    {
        if (m_current_command_offset >= m_current_command_length)
        {
            if (!AdvanceStream())
                break;
        }

        if (m_current_command_length - m_current_command_offset < 1)
            continue;

        const auto result = m_current_command_buffer[m_current_command_offset];
        m_current_command_offset++;
        m_file_offset++;

        return result;
    }

    return EOF;
}

std::streamsize IPakEntryReadStream::xsgetn(char* ptr, const std::streamsize count)
{
    auto* destBuffer = reinterpret_cast<uint8_t*>(ptr);
    std::streamsize countRead = 0;

    while (countRead < count)
    {
        if (m_current_command_offset >= m_current_command_length)
        {
            if (!AdvanceStream())
                break;
        }

        auto sizeToRead = count - countRead;
        sizeToRead = std::min(sizeToRead, static_cast<std::streamsize>(m_current_command_length - m_current_command_offset));

        if (sizeToRead > 0)
        {
            assert(count - countRead >= sizeToRead);
            memcpy(&destBuffer[countRead], &m_current_command_buffer[m_current_command_offset], static_cast<size_t>(sizeToRead));
            countRead += sizeToRead;
            m_current_command_offset += static_cast<size_t>(sizeToRead);
            m_file_offset += sizeToRead;
        }
    }

    return countRead;
}

std::streambuf::pos_type IPakEntryReadStream::seekoff(const off_type off, const std::ios_base::seekdir dir, const std::ios_base::openmode mode)
{
    pos_type pos;
    if (dir == std::ios_base::beg)
    {
        pos = off;
    }
    else if (dir == std::ios_base::cur)
    {
        pos = off + m_file_offset;
    }
    else
    {
        pos = -1;
    }

    if (pos == 0 || pos > m_file_offset)
    {
        return seekpos(pos, mode);
    }

    return std::streampos(-1);
}

std::streambuf::pos_type IPakEntryReadStream::seekpos(const pos_type pos, std::ios_base::openmode mode)
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

        return pos;
    }

    if (pos > m_file_offset)
    {
        while (m_file_head < pos)
        {
            if (!AdvanceStream())
                break;
        }

        if (pos <= m_file_head)
        {
            m_current_command_offset = m_current_command_length - static_cast<size_t>(m_file_head - pos);
            m_file_offset = pos;
        }
        else
        {
            m_current_command_offset = m_current_command_length;
            m_file_offset = m_file_head;
        }

        return pos;
    }

    return std::streampos(-1);
}
