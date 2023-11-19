#pragma once

#include "Utils/ClassUtils.h"
#include "Utils/ObjStream.h"

#include <cstdint>
#include <istream>
#include <mutex>

class IPakStreamManagerActions
{
public:
    virtual void StartReading() = 0;
    virtual void StopReading() = 0;

    virtual void CloseStream(objbuf* stream) = 0;
};

class IPakStreamManager
{
    class Impl;
    Impl* m_impl;

public:
    explicit IPakStreamManager(std::istream& stream);
    IPakStreamManager(const IPakStreamManager& other) = delete;
    IPakStreamManager(IPakStreamManager&& other) noexcept = delete;
    ~IPakStreamManager();

    IPakStreamManager& operator=(const IPakStreamManager& other) = delete;
    IPakStreamManager& operator=(IPakStreamManager&& other) noexcept = delete;

    _NODISCARD std::unique_ptr<iobjstream> OpenStream(int64_t startPosition, size_t length) const;
};
