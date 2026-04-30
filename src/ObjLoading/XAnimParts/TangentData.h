#pragma once

#include "XModel/XModelCommon.h"

#include <array>
#include <vector>

class TangentData
{
public:
    void CreateTangentData(const XModelCommon& common);

    std::vector<std::array<float, 3>> m_tangents;
    std::vector<std::array<float, 3>> m_binormals;
};
