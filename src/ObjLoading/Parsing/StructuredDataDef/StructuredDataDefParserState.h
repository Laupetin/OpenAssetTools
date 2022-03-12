#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstddef>

#include "StructuredDataDef/CommonStructuredDataDef.h"

namespace sdd
{
    class StructuredDataDefParserState
    {
    public:
        std::vector<std::unique_ptr<CommonStructuredDataDef>> m_defs;

        CommonStructuredDataDef* m_current_def;
        CommonStructuredDataEnum* m_current_enum;
        CommonStructuredDataStruct* m_current_struct;
        size_t m_current_struct_offset_in_bits;

        std::map<std::string, CommonStructuredDataType> m_def_types_by_name;
        std::map<CommonStructuredDataIndexedArray, size_t> m_def_indexed_arrays;
        std::map<CommonStructuredDataEnumedArray, size_t> m_def_enumed_arrays;

        StructuredDataDefParserState();
    };
}
