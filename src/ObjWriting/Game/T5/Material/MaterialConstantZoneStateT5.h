#pragma once

#include "Game/T5/T5.h"
#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace T5
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx9
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void AddStaticKnownNames() override;
        [[nodiscard]] unsigned HashString(const std::string& str) const override;
    };
} // namespace T5
