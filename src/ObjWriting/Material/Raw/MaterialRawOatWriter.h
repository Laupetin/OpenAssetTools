#pragma once

#include <ostream>

#include "Dumping/AbstractRawDumper.h"
#include "Material/MaterialCommon.h"

namespace material
{
    class MaterialRawOatWriter final : AbstractRawDumper
    {
    public:
        explicit MaterialRawOatWriter(std::ostream& stream);

        void WriteMaterial(const CommonMaterial& material);
    };
}
    