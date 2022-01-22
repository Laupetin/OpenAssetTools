#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstddef>

#include "Domain/CommonStructuredDataDef.h"

namespace sdd
{
    class StructuredDataDefParserState
    {
    public:
        std::vector<std::unique_ptr<CommonStructuredDataDef>> m_defs;

        CommonStructuredDataDef* m_current_def;
        CommonStructuredDataDefEnum* m_current_enum;
        CommonStructuredDataDefStruct* m_current_struct;
        size_t m_current_struct_offset_in_bits;

        std::map<std::string, CommonStructuredDataDefType> m_def_types_by_name;
        std::map<CommonStructuredDataDefIndexedArray, size_t> m_def_indexed_arrays;
        std::map<CommonStructuredDataDefEnumedArray, size_t> m_def_enumed_arrays;

        StructuredDataDefParserState();
    };
}
