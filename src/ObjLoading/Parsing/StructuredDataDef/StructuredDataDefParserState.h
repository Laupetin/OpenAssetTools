#pragma once
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

        StructuredDataDefParserState();
    };
}
