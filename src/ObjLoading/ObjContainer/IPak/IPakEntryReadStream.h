#pragma once

#include "IPakStreamManager.h"
#include "Utils/FileAPI.h"
#include <mutex>

class IPakEntryReadStream final : public FileAPI::IFile
{
    static constexpr size_t IPAK_CHUNK_SIZE = 0x8000;
    static constexpr size_t IPAK_CHUNK_COUNT_PER_READ = 0x8;

    uint8_t* m_buffer;
    IFile* m_file;
    IPakStreamManager* m_stream_manager;
    std::mutex* m_read_mutex;

    int64_t m_pos;
    int64_t m_base_pos;
    int64_t m_end_pos;
    int64_t m_buffer_pos;

    static int64_t Align(int64_t num, int64_t alignTo);

public:
    IPakEntryReadStream(IFile* file, IPakStreamManager* streamManager, std::mutex* readMutex, int64_t startOffset, size_t length);
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