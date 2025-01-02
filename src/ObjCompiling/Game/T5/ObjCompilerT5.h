#pragma once

#include "IObjCompiler.h"

namespace T5
{
    class ObjCompiler final : public IObjCompiler
    {
    public:
        void ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                        Zone& zone,
                                        const ZoneDefinitionContext& zoneDefinition,
                                        ISearchPath& searchPath,
                                        IGdtQueryable& gdt,
                                        const std::filesystem::path& outDir,
                                        const std::filesystem::path& cacheDir) const override;
    };
} // namespace T5
