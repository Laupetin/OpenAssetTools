#pragma once

#include <istream>

#include "Utils/ObjStream.h"
#include "IPakStreamManager.h"
#include "ObjContainer/IPak/IPakTypes.h"

class IPakEntryReadStream final : public objbuf
{
    static constexpr size_t IPAK_DECOMPRESS_BUFFER_SIZE = 0x8000;

    uint8_t* m_chunk_buffer;

    std::istream& m_stream;
    IPakStreamManagerActions* m_stream_manager_actions;

    int64_t m_file_offset;
    int64_t m_file_head;

    size_t m_entry_size;

    uint8_t m_decompress_buffer[IPAK_DECOMPRESS_BUFFER_SIZE];
    IPakDataBlockHeader* m_current_block;
    unsigned m_next_command;
    uint8_t* m_current_command_buffer;
    size_t m_current_command_length;
    size_t m_current_command_offset;

    int64_t m_pos;
    int64_t m_base_pos;
    int64_t m_end_pos;
    int64_t m_buffer_start_pos;
    int64_t m_buffer_end_pos;

    template<typename T>
    static T AlignForward(const T num, const T alignTo)
    {
        return (num + alignTo - 1) / alignTo * alignTo;
    }

    template<typename T>
    static T AlignBackwards(const T num, const T alignTo)
    {
        return num / alignTo * alignTo;
    }

    size_t ReadChunks(uint8_t* buffer, int64_t startPos, size_t chunkCount) const;
    bool SetChunkBufferWindow(int64_t startPos, size_t chunkCount);
    bool ValidateBlockHeader(IPakDataBlockHeader* blockHeader) const;
    bool AdjustChunkBufferWindowForBlockHeader(IPakDataBlockHeader* blockHeader, size_t blockOffsetInChunk);
    bool NextBlock();
    bool ProcessCommand(size_t commandSize, int compressed);
    bool AdvanceStream();

public:
    IPakEntryReadStream(std::istream& stream, IPakStreamManagerActions* streamManagerActions, uint8_t* chunkBuffer, int64_t startOffset, size_t entrySize);
    ~IPakEntryReadStream() override;

    _NODISCARD bool is_open() const override;
    bool close() override;

protected:
    std::streamsize showmanyc() override;
    int_type underflow() override;
    int_type uflow() override;
    std::streamsize xsgetn(char* ptr, std::streamsize count) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode) override;
    pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override;
};