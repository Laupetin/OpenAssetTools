#include "ObjCompilerIW3.h"

#include "Game/IW3/IW3.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW3;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = zone.Memory();

        // No compilers yet
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection,
                                 Zone& zone,
                                 const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 IOutputPath& outDir)
    {
        auto& memory = zone.Memory();

        if (image::IwdPostProcessor<AssetImage>::AppliesToZoneDefinition(zoneDefinition))
            collection.AddAssetPostProcessor(std::make_unique<image::IwdPostProcessor<AssetImage>>(zoneDefinition, searchPath, zoneStates, outDir));
    }
} // namespace

void ObjCompiler::ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                             Zone& zone,
                                             const ZoneDefinitionContext& zoneDefinition,
                                             ISearchPath& searchPath,
                                             IGdtQueryable& gdt,
                                             ZoneAssetCreationStateContainer& zoneStates,
                                             IOutputPath& outDir,
                                             IOutputPath& cacheDir) const
{
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
