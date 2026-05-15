#pragma once

#include "Utils/ObjStream.h"

#include <cstdint>
#include <istream>
#include <memory>
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
public:
    IPakStreamManager() = default;
    virtual ~IPakStreamManager() = default;

    static std::unique_ptr<IPakStreamManager> Create(std::istream& stream, bool isLittleEndian);

    [[nodiscard]] virtual std::unique_ptr<iobjstream> OpenStream(int64_t startPosition, size_t length) = 0;
};
