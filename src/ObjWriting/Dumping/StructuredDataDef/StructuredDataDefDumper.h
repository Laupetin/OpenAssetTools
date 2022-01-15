#pragma once

#include "Dumping/AbstractTextDumper.h"

class StructuredDataDefDumper : AbstractTextDumper
{
    struct
    {
        bool m_in_version : 1;
        bool m_empty_line_before_version : 1;
    } m_flags;

public:
    explicit StructuredDataDefDumper(std::ostream& stream);

    void BeginVersion(int version);
    void EndVersion();
};
