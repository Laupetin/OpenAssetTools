#include "ObjCompilerT6.h"

#include "Game/T6/T6.h"
#include "Image/ImageIPakPostProcessor.h"
#include "Image/ImageIwdPostProcessor.h"
#include "KeyValuePairs/KeyValuePairsCompilerT6.h"

#include <memory>

using namespace T6;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection,
                            Zone& zone,
                            const ZoneDefinitionContext& zoneDefinition,
                            ISearchPath& searchPath,
                            ZoneAssetCreationStateContainer& zoneStates)
    {
        auto& memory = *zone.GetMemory();

        collection.AddAssetCreator(CreateKeyValuePairsCompiler(memory, zone, zoneDefinition.m_zone_definition, zoneStates));
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection,
                                 Zone& zone,
                                 const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 IOutputPath& outDir)
    {
        auto& memory = *zone.GetMemory();

        if (ImageIPakPostProcessor<AssetImage>::AppliesToZoneDefinition(zoneDefinition))
            collection.AddAssetPostProcessor(std::make_unique<ImageIPakPostProcessor<AssetImage>>(zoneDefinition, searchPath, zoneStates, outDir));

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
                                             IOutputPath& outDir,
                                             IOutputPath& cacheDir) const
{
    ConfigureCompilers(collection, zone, zoneDefinition, searchPath, zoneStates);
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
