#pragma once

#include "Game/T4/T4.h"
#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace T4
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx9
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void AddStaticKnownNames() override;
        [[nodiscard]] unsigned HashString(const std::string& str) const override;
    };
} // namespace T4
