#pragma once

#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace IW4
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx9
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void AddStaticKnownNames() override;
        [[nodiscard]] unsigned HashString(const std::string& str) const override;
    };
} // namespace IW4
