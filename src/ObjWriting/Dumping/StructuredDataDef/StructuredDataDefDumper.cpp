#include "StructuredDataDefDumper.h"

#include <algorithm>
#include <cassert>
#include <sstream>

StructuredDataDefDumper::StructEntry::StructEntry(std::string stringValue, const size_t offset)
    : m_string_value(std::move(stringValue)),
      m_offset(offset)
{
}

StructuredDataDefDumper::StructuredDataDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_block(Block::BLOCK_NONE),
      m_flags{},
      m_enum_entry_count(0u),
      m_struct_property_count(0u),
      m_current_property_offset(0u)
{
}

void StructuredDataDefDumper::BeginVersion(const int version)
{
    assert(!m_flags.m_in_version);
    if (m_flags.m_in_version)
        return;

    if (m_flags.m_empty_line_before_version)
        m_stream << "\n";

    Indent();
    m_stream << "version " << version << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    m_flags.m_in_version = true;
}

void StructuredDataDefDumper::EndVersion()
{
    assert(m_flags.m_in_version);
    if (!m_flags.m_in_version)
        return;

    DecIndent();
    Indent();
    m_stream << "}\n";
    m_flags.m_in_version = false;
    m_flags.m_empty_line_before_version = true;
    m_flags.m_empty_line_before_block = false;
}

void StructuredDataDefDumper::WriteLineComment(const std::string& comment) const
{
    Indent();
    m_stream << "// " << comment << "\n";
}

void StructuredDataDefDumper::BeginEnum(const std::string& enumName, const size_t enumEntryCount, size_t enumReservedEntryCount)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_NONE);

    if (m_block != Block::BLOCK_NONE)
        return;

    if (m_flags.m_empty_line_before_block)
        m_stream << "\n";

    Indent();
    if (enumReservedEntryCount != enumEntryCount)
        m_stream << "enum(" << enumReservedEntryCount << ") ";
    else
        m_stream << "enum ";
     m_stream << enumName;

    m_stream << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    m_block = Block::BLOCK_ENUM;

    m_enum_entries.resize(enumEntryCount);
    m_enum_entry_count = enumEntryCount;
}

void StructuredDataDefDumper::EndEnum()
{
    assert(m_block == Block::BLOCK_ENUM);

    if (m_block != Block::BLOCK_ENUM)
        return;

    bool firstEntry = true;
    for (const auto& entry : m_enum_entries)
    {
        if (firstEntry)
            firstEntry = false;
        else
            m_stream << ",\n";

        Indent();
        m_stream << "\"" << entry << "\"";
    }

    if (!firstEntry)
        m_stream << "\n";

    DecIndent();
    Indent();
    m_stream << "};\n";
    m_block = Block::BLOCK_NONE;
    m_flags.m_empty_line_before_block = true;
    m_enum_entries.clear();
}

void StructuredDataDefDumper::WriteEnumEntry(const std::string& entryName, const size_t entryValue)
{
    assert(m_block == Block::BLOCK_ENUM);
    assert(entryValue < m_enum_entry_count);

    if (m_block != Block::BLOCK_ENUM || entryValue >= m_enum_entry_count)
        return;

    m_enum_entries[entryValue] = entryName;
}

void StructuredDataDefDumper::BeginStruct(const std::string& structName, const size_t structPropertyCount)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_NONE);

    if (m_block != Block::BLOCK_NONE)
        return;

    m_struct_property_count = structPropertyCount;
    m_struct_properties.reserve(structPropertyCount);

    if (m_flags.m_empty_line_before_block)
        m_stream << "\n";

    Indent();
    m_stream << "struct " << structName << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    m_block = Block::BLOCK_STRUCT;
}

void StructuredDataDefDumper::EndStruct()
{
    assert(m_block == Block::BLOCK_STRUCT);

    if (m_block != Block::BLOCK_STRUCT)
        return;

    std::sort(m_struct_properties.begin(), m_struct_properties.end(), [](const StructEntry& e1, const StructEntry& e2)
    {
        return e1.m_offset < e2.m_offset;
    });

    for (auto& structProperty : m_struct_properties)
    {
        Indent();
        m_stream << structProperty.m_string_value << ";\n";
    }

    DecIndent();
    Indent();
    m_stream << "};\n";
    m_block = Block::BLOCK_NONE;
    m_flags.m_empty_line_before_block = true;
    m_struct_properties.clear();
    m_struct_property_count = 0u;
}

void StructuredDataDefDumper::BeginProperty(const std::string& propertyName, const size_t propertyOffset)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_STRUCT);

    if (m_block != Block::BLOCK_STRUCT)
        return;

    m_current_property_name = propertyName;
    m_current_property_offset = propertyOffset;

    m_block = Block::BLOCK_PROPERTY;
}

void StructuredDataDefDumper::AddPropertyArraySpecifier(const std::string& specifierName)
{
    m_current_property_array_specifiers.emplace_back(specifierName);
}

void StructuredDataDefDumper::SetPropertyTypeName(const std::string& typeName)
{
    m_current_property_type_name = typeName;
}

void StructuredDataDefDumper::EndProperty()
{
    assert(m_block == Block::BLOCK_PROPERTY);

    if (m_block != Block::BLOCK_PROPERTY)
        return;

    std::ostringstream ss;
    ss << m_current_property_type_name << " " << m_current_property_name;

    for (const auto& arraySpecifierName : m_current_property_array_specifiers)
    {
        ss << "[" << arraySpecifierName << "]";
    }

    ss << " /* Offset: " << m_current_property_offset / 8;
    
    if (m_current_property_offset % 8 > 0)
        ss << " + " << m_current_property_offset % 8 << "bit";
    
    ss << " */ ";
    m_struct_properties.emplace_back(ss.str(), m_current_property_offset);

    m_block = Block::BLOCK_STRUCT;
    m_current_property_array_specifiers.clear();
    m_current_property_name = std::string();
    m_current_property_offset = 0u;
    m_current_property_type_name = std::string();
}
