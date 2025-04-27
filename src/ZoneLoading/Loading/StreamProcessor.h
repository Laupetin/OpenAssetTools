#pragma once

#include "ILoadingStream.h"

class StreamProcessor : public ILoadingStream
{
public:
    StreamProcessor();

    void SetBaseStream(ILoadingStream* baseStream);

protected:
    ILoadingStream* m_base_stream;
};
