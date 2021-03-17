#pragma once
#include <ostream>

#include "IWritingStream.h"

class WritingFileStream final : public IWritingStream
{
    std::ostream& m_stream;

public:
    explicit WritingFileStream(std::ostream& stream);

    void Write(const void* buffer, size_t length) override;
    void Flush() override;
    int64_t Pos() override;
};
