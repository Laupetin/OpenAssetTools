#pragma once

#include <istream>

#include "ILoadingStream.h"

class LoadingFileStream final : public ILoadingStream
{
    std::istream& m_stream;

public:
    explicit LoadingFileStream(std::istream& stream);

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
