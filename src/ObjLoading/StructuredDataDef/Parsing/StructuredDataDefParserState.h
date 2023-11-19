#pragma once

#include "Parsing/TokenPos.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sdd
{
    class UndefinedType
    {
    public:
        std::string m_name;
        TokenPos m_first_usage_pos;
        CommonStructuredDataType m_mapped_type;

        UndefinedType();
        UndefinedType(std::string name, TokenPos firstUsagePos);
    };

    class StructuredDataDefParserState
    {
    public:
        std::vector<std::unique_ptr<CommonStructuredDataDef>> m_defs;

        CommonStructuredDataDef* m_current_def;
        CommonStructuredDataEnum* m_current_enum;
        CommonStructuredDataStruct* m_current_struct;

        bool m_current_struct_is_root;
        size_t m_current_struct_padding_offset;

        size_t m_checksum_override_value;
        bool m_checksum_overriden;

        std::map<std::string, CommonStructuredDataType> m_def_types_by_name;
        std::map<CommonStructuredDataIndexedArray, size_t> m_def_indexed_arrays;
        std::map<CommonStructuredDataEnumedArray, size_t> m_def_enumed_arrays;
        std::vector<UndefinedType> m_undefined_types;

        StructuredDataDefParserState();
    };
} // namespace sdd
