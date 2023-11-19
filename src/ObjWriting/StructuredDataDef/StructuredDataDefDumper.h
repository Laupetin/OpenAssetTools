#pragma once
#include "Dumping/AbstractTextDumper.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <ostream>
#include <string>
#include <vector>

class StructuredDataDefDumper : AbstractTextDumper
{
    struct
    {
        bool m_empty_line_before_definition : 1;
    } m_flags;

    void WriteLineComment(const std::string& comment) const;

    void DumpEnum(const CommonStructuredDataEnum& _enum);
    void DumpType(const CommonStructuredDataDef& def, CommonStructuredDataType type, std::string& typeName, std::vector<std::string>& arraySpecifiers) const;
    void DumpProperty(const CommonStructuredDataDef& def, const CommonStructuredDataStructProperty& property, unsigned& currentOffsetInBit) const;
    void DumpStruct(const CommonStructuredDataDef& def, const CommonStructuredDataStruct& _struct, size_t structIndex);

public:
    explicit StructuredDataDefDumper(std::ostream& stream);

    void DumpDef(const CommonStructuredDataDef& def);
};
