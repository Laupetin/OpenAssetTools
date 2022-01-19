#pragma once
#include <map>
#include <memory>
#include <vector>

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

        std::map<std::string, CommonStructuredDataDefType> m_def_types_by_name;
        std::map<CommonStructuredDataDefIndexedArray, size_t> m_def_indexed_arrays;
        std::map<CommonStructuredDataDefEnumedArray, size_t> m_def_enumed_arrays;

        StructuredDataDefParserState();
    };
}
