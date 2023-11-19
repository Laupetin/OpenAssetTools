#pragma once

#include "StructuredDataDef/CommonStructuredDataDef.h"
#include "Utils/ClassUtils.h"

#include <exception>
#include <string>

namespace sdd
{
    class SizeCalculationException final : std::exception
    {
        std::string m_message;

    public:
        explicit SizeCalculationException(std::string message);

        _NODISCARD const std::string& Message() const;
        _NODISCARD char const* what() const noexcept override;
    };

    class StructuredDataDefSizeCalculator
    {
    public:
        static void CalculateSizesAndOffsetsForDef(CommonStructuredDataDef& def);
    };
} // namespace sdd
