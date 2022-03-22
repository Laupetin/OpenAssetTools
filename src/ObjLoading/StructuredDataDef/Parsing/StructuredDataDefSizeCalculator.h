#pragma once

#include <string>
#include <exception>

#include "Utils/ClassUtils.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

namespace sdd
{
    class SizeCalculationException final : std::exception
    {
        std::string m_message;

    public:
        explicit SizeCalculationException(std::string message);

        _NODISCARD const std::string& Message() const;
        _NODISCARD char const* what() const override;
    };

    class StructuredDataDefSizeCalculator
    {
    public:
        static void CalculateSizesAndOffsetsForDef(CommonStructuredDataDef& def);
    };
}
