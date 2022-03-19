#include "StructuredDataDefDumper.h"

StructuredDataDefDumperNew::StructuredDataDefDumperNew(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_flags{}
{
}

void StructuredDataDefDumperNew::WriteLineComment(const std::string& comment) const
{
    Indent();
    m_stream << "// " << comment << "\n";
}

void StructuredDataDefDumperNew::DumpEnum(const CommonStructuredDataEnum& _enum)
{
    Indent();

    if (_enum.m_reserved_entry_count > static_cast<int>(_enum.m_entries.size()))
        m_stream << "enum(" << _enum.m_reserved_entry_count << ") ";
    else
        m_stream << "enum ";

    m_stream << _enum.m_name << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();

    const auto entryCount = _enum.m_entries.size();
    for(auto i = 0u; i < entryCount; i++)
    {
        Indent();
        m_stream << "\"" << _enum.m_entries[i].m_name << "\"";

        if (i + 1 < entryCount)
            m_stream << ",";
        m_stream << "\n";
    }

    DecIndent();
    Indent();
    m_stream << "};\n"; // end enum
}

void StructuredDataDefDumperNew::DumpStruct(const CommonStructuredDataStruct& _struct)
{
    Indent();

    m_stream << "struct " << _struct.m_name << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();


    DecIndent();
    Indent();
    m_stream << "};\n"; // end struct
}

void StructuredDataDefDumperNew::DumpDef(const CommonStructuredDataDef& def)
{
    if (m_flags.m_empty_line_before_definition)
        m_stream << "\n\n";
    else
        m_flags.m_empty_line_before_definition = true;

    const auto calculatedChecksum = def.CalculateChecksum();

    m_stream << "// ====================\n";
    m_stream << "// Version " << def.m_version << "\n";

    if (calculatedChecksum != def.m_checksum)
        m_stream << "// Calculated checksum did not match checksum in file\n// Overriding checksum to match original value\n";

    m_stream << "// ====================\n";

    m_stream << "version " << def.m_version << "\n{\n";
    IncIndent();

    auto insertEmptyLine = false;

    if (calculatedChecksum != def.m_checksum)
    {
        Indent();
        m_stream << "checksumoverride " << def.m_checksum << ";\n";
        insertEmptyLine = true;
    }

    for(const auto& _enum : def.m_enums)
    {
        if (insertEmptyLine)
            m_stream << "\n";
        else
            insertEmptyLine = true;

        DumpEnum(*_enum);
    }

    for(const auto& _struct : def.m_structs)
    {
        if (insertEmptyLine)
            m_stream << "\n";
        else
            insertEmptyLine = true;

        DumpStruct(*_struct);
    }

    DecIndent();
    m_stream << "}\n"; // end version
}
