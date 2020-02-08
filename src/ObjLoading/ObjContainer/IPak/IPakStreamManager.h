#pragma once

#include "Utils/FileAPI.h"
#include <vector>
#include <mutex>

class IPakStreamManager
{
    FileAPI::IFile* m_file;
    std::vector<FileAPI::IFile*> m_open_streams;
    std::mutex m_read_mutex;
    std::mutex m_stream_mutex;

public:
    explicit IPakStreamManager(FileAPI::IFile* file);
    IPakStreamManager(const IPakStreamManager& other) = delete;
    IPakStreamManager(IPakStreamManager&& other) noexcept = delete;
    ~IPakStreamManager();

    IPakStreamManager& operator=(const IPakStreamManager& other) = delete;
    IPakStreamManager& operator=(IPakStreamManager&& other) noexcept = delete;

    FileAPI::IFile* OpenStream(int64_t startPosition, size_t length);
    void OnCloseStream(FileAPI::IFile* stream);
};