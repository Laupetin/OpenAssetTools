#pragma once

#include "ILoadingStream.h"

#include <cstdint>
#include <istream>

class LoadingFileStream final : public ILoadingStream
{
public:
    explicit LoadingFileStream(std::istream& stream);

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;

private:
    std::istream& m_stream;
};
