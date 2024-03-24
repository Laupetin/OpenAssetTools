#include "CommonStructuredDataStruct.h"

#include "CommonStructuredDataDef.h"
#include "Utils/Endianness.h"

#include <algorithm>
#include <zlib.h>

CommonStructuredDataStructProperty::CommonStructuredDataStructProperty()
    : m_offset_in_bits(0u)
{
}

CommonStructuredDataStructProperty::CommonStructuredDataStructProperty(std::string name)
    : m_name(std::move(name)),
      m_offset_in_bits(0u)
{
}

CommonStructuredDataStructProperty::CommonStructuredDataStructProperty(std::string name, const CommonStructuredDataType type, const size_t offsetInBits)
    : m_name(std::move(name)),
      m_type(type),
      m_offset_in_bits(offsetInBits)
{
}

CommonStructuredDataStruct::CommonStructuredDataStruct()
    : m_bit_offset(0u),
      m_size_in_byte(0u)
{
}

CommonStructuredDataStruct::CommonStructuredDataStruct(std::string name)
    : m_name(std::move(name)),
      m_bit_offset(0u),
      m_size_in_byte(0u)
{
}

uint32_t CommonStructuredDataStruct::CalculateChecksum(const CommonStructuredDataDef& def, const uint32_t initialValue) const
{
    auto checksum = initialValue;

    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(m_name.c_str()), m_name.size() + 1);
    for (const auto& property : m_properties)
    {
        checksum = crc32(checksum, reinterpret_cast<const Bytef*>(property.m_name.c_str()), property.m_name.size() + 1);

        const auto littleEndianOffset = endianness::ToLittleEndian(property.m_offset_in_bits);
        checksum = crc32(checksum, reinterpret_cast<const Bytef*>(&littleEndianOffset), sizeof(littleEndianOffset));

        auto currentType = property.m_type;
        while (currentType.m_category != CommonStructuredDataTypeCategory::UNKNOWN)
        {
            const auto categoryByte = static_cast<uint8_t>(currentType.m_category);
            checksum = crc32(checksum, &categoryByte, sizeof(categoryByte));

            switch (currentType.m_category)
            {
            case CommonStructuredDataTypeCategory::STRING:
            {
                const auto littleEndianStringLength = endianness::ToLittleEndian(currentType.m_info.string_length);
                checksum = crc32(checksum, reinterpret_cast<const Bytef*>(&littleEndianStringLength), sizeof(littleEndianStringLength));
                currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
            }
            break;
            case CommonStructuredDataTypeCategory::ENUM:
                if (currentType.m_info.type_index < def.m_enums.size())
                {
                    const auto& _enum = *def.m_enums[currentType.m_info.type_index];
                    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(_enum.m_name.c_str()), _enum.m_name.size() + 1);
                    currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
                }
                break;
            case CommonStructuredDataTypeCategory::STRUCT:
                if (currentType.m_info.type_index < def.m_structs.size())
                {
                    const auto& _struct = *def.m_structs[currentType.m_info.type_index];
                    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(_struct.m_name.c_str()), _struct.m_name.size() + 1);
                    currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
                }
                break;
            case CommonStructuredDataTypeCategory::INDEXED_ARRAY:
                if (currentType.m_info.type_index < def.m_indexed_arrays.size())
                {
                    const auto& indexedArray = def.m_indexed_arrays[currentType.m_info.type_index];
                    const auto littleEndianElementCount = endianness::ToLittleEndian(indexedArray.m_element_count);
                    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(&littleEndianElementCount), sizeof(littleEndianElementCount));
                    currentType = indexedArray.m_array_type;
                }
                else
                    currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
                break;
            case CommonStructuredDataTypeCategory::ENUM_ARRAY:
                if (currentType.m_info.type_index < def.m_enumed_arrays.size())
                {
                    const auto& enumedArray = def.m_enumed_arrays[currentType.m_info.type_index];

                    if (enumedArray.m_enum_index < def.m_enums.size())
                    {
                        const auto& _enum = *def.m_enums[enumedArray.m_enum_index];
                        checksum = crc32(checksum, reinterpret_cast<const Bytef*>(_enum.m_name.c_str()), _enum.m_name.size() + 1);
                    }
                    currentType = enumedArray.m_array_type;
                }
                else
                    currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
                break;
            default:
                currentType = CommonStructuredDataType(CommonStructuredDataTypeCategory::UNKNOWN);
                break;
            }
        }
    }

    return checksum;
}

void CommonStructuredDataStruct::SortPropertiesByOffset()
{
    std::ranges::sort(m_properties,
                      [](const CommonStructuredDataStructProperty& e1, const CommonStructuredDataStructProperty& e2)
                      {
                          return e1.m_offset_in_bits < e2.m_offset_in_bits;
                      });
}

void CommonStructuredDataStruct::SortPropertiesByName()
{
    std::ranges::sort(m_properties,
                      [](const CommonStructuredDataStructProperty& e1, const CommonStructuredDataStructProperty& e2)
                      {
                          return e1.m_name < e2.m_name;
                      });
}
