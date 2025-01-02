#pragma once

#include "IObjCompiler.h"

namespace IW3
{
    class ObjCompiler final : public IObjCompiler
    {
    public:
        void ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                        Zone& zone,
                                        const ZoneDefinition& zoneDefinition,
                                        ISearchPath& searchPath,
                                        IGdtQueryable& gdt,
                                        const std::filesystem::path& outDir,
                                        const std::filesystem::path& cacheDir) const override;
    };
} // namespace IW3
