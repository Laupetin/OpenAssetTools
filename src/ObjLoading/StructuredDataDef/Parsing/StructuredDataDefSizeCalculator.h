#pragma once

#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <exception>
#include <string>

namespace sdd
{
    class SizeCalculationException final : std::exception
    {
        std::string m_message;

    public:
        explicit SizeCalculationException(std::string message);

        [[nodiscard]] const std::string& Message() const;
        [[nodiscard]] char const* what() const noexcept override;
    };

    class StructuredDataDefSizeCalculator
    {
    public:
        static void CalculateSizesAndOffsetsForDef(CommonStructuredDataDef& def);
    };
} // namespace sdd
