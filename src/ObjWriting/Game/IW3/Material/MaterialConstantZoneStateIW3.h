#pragma once

#include "Game/IW3/IW3.h"
#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace IW3
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx9
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void AddStaticKnownNames() override;
        [[nodiscard]] unsigned HashString(const std::string& str) const override;
    };
} // namespace IW3
