#pragma once

#include "IPakStreamManager.h"
#include "Utils/FileAPI.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include <mutex>

class IPakEntryReadStream final : public FileAPI::IFile
{
    static constexpr size_t IPAK_DECOMPRESS_BUFFER_SIZE = 0x8000;

    uint8_t* m_chunk_buffer;

    IFile* m_file;
    IPakStreamManagerActions* m_stream_manager_actions;

    size_t m_file_offset;
    size_t m_file_head;

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
    IPakEntryReadStream(IFile* file, IPakStreamManagerActions* streamManagerActions, uint8_t* chunkBuffer, int64_t startOffset, size_t entrySize);
    ~IPakEntryReadStream() override;

    bool IsOpen() override;
    size_t Read(void* buffer, size_t elementSize, size_t elementCount) override;
    size_t Write(const void* data, size_t elementSize, size_t elementCount) override;
    void Skip(int64_t amount) override;
    size_t Printf(const char* fmt, ...) override;
    int64_t Pos() override;
    void Goto(int64_t pos) override;
    void GotoEnd() override;
    void Close() override;
};