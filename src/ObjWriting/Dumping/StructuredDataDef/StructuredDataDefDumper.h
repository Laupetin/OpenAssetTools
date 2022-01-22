#pragma once

#include <vector>

#include "Dumping/AbstractTextDumper.h"

class StructuredDataDefDumper : AbstractTextDumper
{
    struct StructEntry
    {
        std::string m_string_value;
        size_t m_offset;

        StructEntry(std::string stringValue, size_t offset);
    };

    enum class Block
    {
        BLOCK_NONE = 0,
        BLOCK_ENUM = 1,
        BLOCK_STRUCT = 2,
        BLOCK_PROPERTY = 3
    } m_block;

    struct
    {
        bool m_in_version : 1;
        bool m_empty_line_before_version : 1;
        bool m_empty_line_before_block : 1;
    } m_flags;

    std::vector<std::string> m_enum_entries;
    size_t m_enum_entry_count;

    std::vector<StructEntry> m_struct_properties;
    size_t m_struct_property_count;
    std::string m_current_property_name;
    size_t m_current_property_offset;
    std::string m_current_property_type_name;
    std::vector<std::string> m_current_property_array_specifiers;

public:
    explicit StructuredDataDefDumper(std::ostream& stream);

    void BeginVersion(int version);
    void EndVersion();

    void WriteLineComment(const std::string& comment) const;

    void BeginEnum(const std::string& enumName, size_t enumEntryCount, size_t enumReservedEntryCount);
    void EndEnum();
    void WriteEnumEntry(const std::string& entryName, size_t entryValue);

    void BeginStruct(const std::string& structName, size_t structPropertyCount);
    void EndStruct();
    void BeginProperty(const std::string& propertyName, size_t propertyOffset);
    void AddPropertyArraySpecifier(const std::string& specifierName);
    void SetPropertyTypeName(const std::string& typeName);
    void EndProperty();
};
