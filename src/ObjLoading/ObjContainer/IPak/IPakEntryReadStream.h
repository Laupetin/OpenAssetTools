#pragma once

#include "IPakStreamManager.h"
#include "Utils/FileAPI.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include <mutex>

class IPakEntryReadStream final : public FileAPI::IFile
{
    uint8_t* m_chunk_buffer;
    uint8_t* m_file_buffer;

    IFile* m_file;
    IPakStreamManagerActions* m_stream_manager_actions;

    size_t m_file_offset;
    size_t m_file_head;

    size_t m_entry_size;

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

    bool SetChunkBufferWindow(int64_t startPos, size_t chunkCount);
    bool ValidateBlockHeader(IPakDataBlockHeader* blockHeader) const;
    bool AdjustChunkBufferWindowForBlockHeader(IPakDataBlockHeader* blockHeader, size_t blockOffsetInChunk);
    bool ProcessCommand(size_t commandSize, bool compressed);
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