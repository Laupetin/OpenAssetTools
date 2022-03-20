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
    for (auto i = 0u; i < entryCount; i++)
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

void StructuredDataDefDumperNew::DumpType(const CommonStructuredDataDef& def, CommonStructuredDataType type, std::string& typeName, std::vector<std::string>& arraySpecifiersInReverseOrder)
{
    typeName = "unknown";
}

void StructuredDataDefDumperNew::DumpProperty(const CommonStructuredDataDef& def, const CommonStructuredDataStructEntry& property, unsigned& currentOffsetInBit)
{
    std::string typeName;
    std::vector<std::string> arraySpecifiersInReverseOrder;

    DumpType(def, property.m_type, typeName, arraySpecifiersInReverseOrder);

    Indent();
    m_stream << typeName << ' ' << property.m_name;

    for (auto ri = arraySpecifiersInReverseOrder.rbegin(); ri != arraySpecifiersInReverseOrder.rend(); ++ri)
        m_stream << '[' << *ri << ']';

#ifdef STRUCTUREDDATADEF_DEBUG
    m_stream << " /* Offset: " << (property.m_offset_in_bits / 8) << " byte";
    if (property.m_offset_in_bits % 8 > 0)
        m_stream << " + " << (property.m_offset_in_bits % 8) << " bit";
    m_stream << " */";
#endif

    m_stream << ";\n";
}

void StructuredDataDefDumperNew::DumpStruct(const CommonStructuredDataDef& def, const CommonStructuredDataStruct& _struct, const size_t structIndex)
{
#ifdef STRUCTUREDDATADEF_DEBUG
    Indent();
    m_stream << "// BitOffset: " << _struct.m_bit_offset << "\n";
    Indent();
    m_stream << "// Size (Byte): " << _struct.m_size_in_byte << "\n";
#endif

    Indent();

    m_stream << "struct " << _struct.m_name << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();

    auto currentOffsetInBit = def.m_root_type.m_category == CommonStructuredDataTypeCategory::STRUCT && def.m_root_type.m_info.type_index == structIndex ? 64u : 0u;
    for (const auto& property : _struct.m_properties)
        DumpProperty(def, property, currentOffsetInBit);

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

#ifdef STRUCTUREDDATADEF_DEBUG
    m_stream << "// Size (Byte): " << def.m_size_in_byte << "\n";
    m_stream << "// Checksum: " << def.m_checksum << "\n";
#endif

    m_stream << "version " << def.m_version << "\n{\n";
    IncIndent();

    auto insertEmptyLine = false;

    if (calculatedChecksum != def.m_checksum)
    {
        Indent();
        m_stream << "checksumoverride " << def.m_checksum << ";\n";
        insertEmptyLine = true;
    }

    for (const auto& _enum : def.m_enums)
    {
        if (insertEmptyLine)
            m_stream << "\n";
        else
            insertEmptyLine = true;

        DumpEnum(*_enum);
    }

    auto structIndex = 0u;
    for (const auto& _struct : def.m_structs)
    {
        if (insertEmptyLine)
            m_stream << "\n";
        else
            insertEmptyLine = true;

        DumpStruct(def, *_struct, structIndex++);
    }

    DecIndent();
    m_stream << "}\n"; // end version
}
