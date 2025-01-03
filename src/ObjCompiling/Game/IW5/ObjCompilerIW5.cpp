#include "ObjCompilerIW5.h"

#include "Game/IW5/IW5.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW5;
namespace fs = std::filesystem;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = *zone.GetMemory();

        // No compilers yet
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection,
                                 Zone& zone,
                                 const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 const fs::path& outDir)
    {
        auto& memory = *zone.GetMemory();

        if (ImageIwdPostProcessor<AssetImage>::AppliesToZoneDefinition(zoneDefinition))
            collection.AddAssetPostProcessor(std::make_unique<ImageIwdPostProcessor<AssetImage>>(zoneDefinition, searchPath, zoneStates, outDir));
    }
} // namespace

void ObjCompiler::ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                             Zone& zone,
                                             const ZoneDefinitionContext& zoneDefinition,
                                             ISearchPath& searchPath,
                                             IGdtQueryable& gdt,
                                             ZoneAssetCreationStateContainer& zoneStates,
                                             const fs::path& outDir,
                                             const fs::path& cacheDir) const
{
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
