#pragma once

#include "XAnim/XAnimCommon.h"

#include <cstdint>
#include <vector>

namespace xanim
{
    class FlatData
    {
    public:
        std::vector<uint8_t> m_data_byte;
        std::vector<int16_t> m_data_short;
        std::vector<int32_t> m_data_int;
        std::vector<uint8_t> m_random_data_byte;
        std::vector<int16_t> m_random_data_short;
        std::vector<uint16_t> m_indices;
    };

    FlatData CreateFlatDataFromCommonXAnim(const CommonXAnimParts& parts);
} // namespace xanim
