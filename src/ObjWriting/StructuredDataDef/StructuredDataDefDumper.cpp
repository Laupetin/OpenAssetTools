#include "StructuredDataDefDumper.h"

#include <cassert>
#include <sstream>

#include "Utils/Alignment.h"
#include "Utils/StringUtils.h"

StructuredDataDefDumper::StructuredDataDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_flags{}
{
}

void StructuredDataDefDumper::WriteLineComment(const std::string& comment) const
{
    Indent();
    m_stream << "// " << comment << "\n";
}

void StructuredDataDefDumper::DumpEnum(const CommonStructuredDataEnum& _enum)
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
        m_stream << "\"";
        utils::EscapeStringForQuotationMarks(m_stream, _enum.m_entries[i].m_name);
        m_stream << "\"";

        if (i + 1 < entryCount)
            m_stream << ",";
        m_stream << "\n";
    }

    DecIndent();
    Indent();
    m_stream << "};\n"; // end enum
}

void StructuredDataDefDumper::DumpType(const CommonStructuredDataDef& def, CommonStructuredDataType type, std::string& typeName, std::vector<std::string>& arraySpecifiers) const
{
    while (type.m_category != CommonStructuredDataTypeCategory::UNKNOWN)
    {
        switch (type.m_category)
        {
        case CommonStructuredDataTypeCategory::INT:
            typeName = "int";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::BYTE:
            typeName = "byte";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::BOOL:
            typeName = "bool";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::FLOAT:
            typeName = "float";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::SHORT:
            typeName = "short";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::STRING:
            {
                std::ostringstream ss;
                ss << "string(" << type.m_info.string_length << ')';
                typeName = ss.str();
                type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            }
            break;
        case CommonStructuredDataTypeCategory::ENUM:
            assert(type.m_info.type_index < def.m_enums.size());
            if (type.m_info.type_index < def.m_enums.size())
                typeName = def.m_enums[type.m_info.type_index]->m_name;
            else
                typeName = "unknown";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::STRUCT:
            assert(type.m_info.type_index < def.m_structs.size());
            if (type.m_info.type_index < def.m_structs.size())
                typeName = def.m_structs[type.m_info.type_index]->m_name;
            else
                typeName = "unknown";
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::INDEXED_ARRAY:
            assert(type.m_info.type_index < def.m_indexed_arrays.size());
            if (type.m_info.type_index < def.m_indexed_arrays.size())
            {
                const auto& indexArray = def.m_indexed_arrays[type.m_info.type_index];
                arraySpecifiers.push_back(std::to_string(indexArray.m_element_count));
                type = indexArray.m_array_type;
            }
            else
                type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        case CommonStructuredDataTypeCategory::ENUM_ARRAY:
            assert(type.m_info.type_index < def.m_enumed_arrays.size());
            if (type.m_info.type_index < def.m_enumed_arrays.size())
            {
                const auto& enumedArray = def.m_enumed_arrays[type.m_info.type_index];

                assert(enumedArray.m_enum_index < def.m_enums.size());
                if (enumedArray.m_enum_index < def.m_enums.size())
                    arraySpecifiers.push_back(def.m_enums[enumedArray.m_enum_index]->m_name);
                type = enumedArray.m_array_type;
            }
            else
                type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        default:
            type = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            break;
        }
    }
}

void StructuredDataDefDumper::DumpProperty(const CommonStructuredDataDef& def, const CommonStructuredDataStructProperty& property, unsigned& currentOffsetInBit) const
{
    std::string typeName;
    std::vector<std::string> arraySpecifiers;

    currentOffsetInBit = utils::Align(currentOffsetInBit, property.m_type.GetAlignmentInBits());
    assert(currentOffsetInBit <= property.m_offset_in_bits);
    if (currentOffsetInBit < property.m_offset_in_bits)
    {
        assert((property.m_offset_in_bits - currentOffsetInBit) % 8 == 0);
        Indent();
        m_stream << "pad(" << ((property.m_offset_in_bits - currentOffsetInBit) / 8) << ");\n";
        currentOffsetInBit = property.m_offset_in_bits;
    }

    DumpType(def, property.m_type, typeName, arraySpecifiers);

    Indent();
    m_stream << typeName << ' ' << property.m_name;

    for (auto ri = arraySpecifiers.begin(); ri != arraySpecifiers.end(); ++ri)
        m_stream << '[' << *ri << ']';

#ifdef STRUCTUREDDATADEF_DEBUG
    m_stream << "; // Offset: " << (property.m_offset_in_bits / 8) << " byte";
    if (property.m_offset_in_bits % 8 > 0)
        m_stream << " + " << (property.m_offset_in_bits % 8) << " bit";

    const auto sizeInBits = property.m_type.GetSizeInBits(def);
    m_stream << " | Size: ";

    if (sizeInBits % 8 == 0)
        m_stream << (sizeInBits / 8) << " byte";
    else if (sizeInBits > 8)
        m_stream << (sizeInBits / 8) << " byte + " << (sizeInBits % 8) << " bit";
    else
        m_stream << sizeInBits << " bit";

    m_stream << " | Alignment: " << property.m_type.GetAlignmentInBits() << " bit";
    m_stream << '\n';
#else
    m_stream << ";\n";
#endif

    currentOffsetInBit += property.m_type.GetSizeInBits(def);
}

void StructuredDataDefDumper::DumpStruct(const CommonStructuredDataDef& def, const CommonStructuredDataStruct& _struct, const size_t structIndex)
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

    currentOffsetInBit = utils::Align(currentOffsetInBit, 8u);
    if ((currentOffsetInBit / 8) < _struct.m_size_in_byte)
    {
        Indent();
        m_stream << "pad(" << (_struct.m_size_in_byte - (currentOffsetInBit / 8)) << ");\n";
    }

    DecIndent();
    Indent();
    m_stream << "};\n"; // end struct
}

void StructuredDataDefDumper::DumpDef(const CommonStructuredDataDef& def)
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
