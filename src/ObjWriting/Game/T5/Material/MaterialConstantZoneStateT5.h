#pragma once

#include "Game/T5/T5.h"
#include "Material/AbstractMaterialConstantZoneState.h"

#include <string>

namespace T5
{
    class MaterialConstantZoneState final : public AbstractMaterialConstantZoneStateDx11
    {
    protected:
        void ExtractNamesFromZoneInternal() override;
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void AddStaticKnownNames() override;
        unsigned HashString(const std::string& str) override;
    };
} // namespace T5
