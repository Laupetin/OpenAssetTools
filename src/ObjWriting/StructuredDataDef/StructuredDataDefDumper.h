#pragma once
#include <ostream>

#include "Dumping/AbstractTextDumper.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

class StructuredDataDefDumperNew : AbstractTextDumper
{
    struct
    {
        bool m_empty_line_before_definition : 1;
    } m_flags;

    void WriteLineComment(const std::string& comment) const;

    void DumpEnum(const CommonStructuredDataEnum& _enum);
    void DumpStruct(const CommonStructuredDataStruct& _struct);

public:
    explicit StructuredDataDefDumperNew(std::ostream& stream);

    void DumpDef(const CommonStructuredDataDef& def);
};
