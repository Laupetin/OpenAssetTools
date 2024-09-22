#pragma once

#include "Game/T6/T6.h"
#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace T6
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx11
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void AddStaticKnownNames() override;
        unsigned HashString(const std::string& str) override;
    };
} // namespace T6
