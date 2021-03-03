#pragma once

#include "ILoadingStream.h"

class StreamProcessor : public ILoadingStream
{
protected:
    ILoadingStream* m_base_stream;

public:
    StreamProcessor();

    void SetBaseStream(ILoadingStream* baseStream);
};
