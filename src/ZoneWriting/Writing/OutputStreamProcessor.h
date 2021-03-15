#pragma once

#include "IWritingStream.h"

class OutputStreamProcessor : public IWritingStream
{
protected:
    IWritingStream* m_base_stream;

public:
    OutputStreamProcessor();

    void SetBaseStream(IWritingStream* baseStream);
};
