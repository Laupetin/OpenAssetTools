#pragma once

#include <memory>
#include <vector>

#include "CommonStructuredDataDefEnum.h"
#include "CommonStructuredDataDefStruct.h"

class CommonStructuredDataDef
{
public:
    std::vector<std::unique_ptr<CommonStructuredDataDefEnum>> m_enums;
    std::vector< std::unique_ptr<CommonStructuredDataDefStruct>> m_structs;
    std::vector<CommonStructuredDataDefIndexedArray> m_indexed_arrays;
    std::vector<CommonStructuredDataDefEnumedArray> m_enumed_arrays;

    int m_version;
    size_t m_checksum;
    CommonStructuredDataDefType m_root_type;
    size_t m_size_in_byte;

    CommonStructuredDataDef();
    explicit CommonStructuredDataDef(int version);
};
