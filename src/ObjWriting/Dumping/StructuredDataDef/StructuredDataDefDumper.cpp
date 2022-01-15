#include "StructuredDataDefDumper.h"

#include <cassert>

StructuredDataDefDumper::StructuredDataDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_block(Block::BLOCK_NONE),
      m_flags{},
      m_enum_size(0u)
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

void StructuredDataDefDumper::BeginEnum(const std::string& enumName, const size_t enumEntryCount)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_NONE);

    if (m_block != Block::BLOCK_NONE)
        return;

    if (m_flags.m_empty_line_before_block)
        m_stream << "\n";

    Indent();
    m_stream << "enum " << enumName << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    m_block = Block::BLOCK_ENUM;

    m_enum_entries.resize(enumEntryCount);
    m_enum_size = enumEntryCount;
}

void StructuredDataDefDumper::EndEnum()
{
    assert(m_block == Block::BLOCK_ENUM);

    if (m_block != Block::BLOCK_ENUM)
        return;

    bool firstEntry = true;
    for(const auto& entry : m_enum_entries)
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
    assert(entryValue < m_enum_size);

    if (m_block != Block::BLOCK_ENUM || entryValue >= m_enum_size)
        return;

    m_enum_entries[entryValue] = entryName;
}

void StructuredDataDefDumper::BeginStruct(const std::string& structName)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_NONE);

    if (m_block != Block::BLOCK_NONE)
        return;

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

    DecIndent();
    Indent();
    m_stream << "};\n";
    m_block = Block::BLOCK_NONE;
    m_flags.m_empty_line_before_block = true;
}

void StructuredDataDefDumper::BeginProperty(const std::string& propertyName)
{
    assert(m_flags.m_in_version);
    assert(m_block == Block::BLOCK_STRUCT);

    if (m_block != Block::BLOCK_STRUCT)
        return;

    m_property_name = propertyName;

    m_block = Block::BLOCK_PROPERTY;
}

void StructuredDataDefDumper::AddPropertyArraySpecifier(const std::string& specifierName)
{
    m_property_array_specifiers.emplace_back(specifierName);
}

void StructuredDataDefDumper::SetPropertyTypeName(const std::string& typeName)
{
    m_property_type_name = typeName;
}

void StructuredDataDefDumper::EndProperty()
{
    assert(m_block == Block::BLOCK_PROPERTY);

    if (m_block != Block::BLOCK_PROPERTY)
        return;

    Indent();

    m_stream << m_property_type_name << " " << m_property_name;

    for(const auto& arraySpecifierName : m_property_array_specifiers)
    {
        m_stream << "[" << arraySpecifierName << "]";
    }

    m_stream << ";\n";

    m_block = Block::BLOCK_STRUCT;
    m_property_array_specifiers.clear();
    m_property_name = std::string();
    m_property_type_name = std::string();
}
