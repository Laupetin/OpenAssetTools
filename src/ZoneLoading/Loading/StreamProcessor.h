#pragma once

#include "ILoadingStream.h"

class StreamProcessor : public ILoadingStream
{
public:
    StreamProcessor();

    void SetBaseStream(ILoadingStream* baseStream);

    void Skip(size_t length) override;
    int64_t Pos() override;

protected:
    ILoadingStream* m_base_stream;
};
