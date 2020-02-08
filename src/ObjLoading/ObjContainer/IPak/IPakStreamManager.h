#pragma once

#include "Utils/FileAPI.h"
#include <cstdint>
#include <mutex>

class IPakStreamManagerActions
{
public:
    virtual void StartReading() = 0;
    virtual void StopReading() = 0;

    virtual void CloseStream(FileAPI::IFile* stream) = 0;
};

class IPakStreamManager
{
    class Impl;
    Impl* m_impl;

public:
    explicit IPakStreamManager(FileAPI::IFile* file);
    IPakStreamManager(const IPakStreamManager& other) = delete;
    IPakStreamManager(IPakStreamManager&& other) noexcept = delete;
    ~IPakStreamManager();

    IPakStreamManager& operator=(const IPakStreamManager& other) = delete;
    IPakStreamManager& operator=(IPakStreamManager&& other) noexcept = delete;

    FileAPI::IFile* OpenStream(int64_t startPosition, size_t length) const;
};