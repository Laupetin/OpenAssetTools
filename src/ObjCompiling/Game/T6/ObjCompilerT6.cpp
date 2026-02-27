#include "ObjCompilerT6.h"

#include "Game/T6/T6.h"
#include "Image/ImageIPakPostProcessor.h"
#include "Image/ImageIwdPostProcessor.h"
#include "KeyValuePairs/KeyValuePairsCompilerT6.h"
#include "Techset/TechniqueCompilerT6.h"
#include "Techset/TechsetCompilerT6.h"
#include "Techset/VertexDeclCompilerT6.h"

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
        auto& memory = zone.Memory();

        collection.AddAssetCreator(key_value_pairs::CreateCompilerT6(memory, zone, zoneDefinition.m_zone_definition, zoneStates));
        collection.AddAssetCreator(techset::CreateCompilerT6(memory, searchPath));

        collection.AddSubAssetCreator(techset::CreateTechniqueCompilerT6(memory, searchPath));
        collection.AddSubAssetCreator(techset::CreateVertexDeclCompilerT6(memory));
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection,
                                 Zone& zone,
                                 const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 IOutputPath& outDir)
    {
        auto& memory = zone.Memory();

        if (image::IPakPostProcessor<AssetImage>::AppliesToZoneDefinition(zoneDefinition))
            collection.AddAssetPostProcessor(std::make_unique<image::IPakPostProcessor<AssetImage>>(zoneDefinition, searchPath, zoneStates, outDir));

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
    ConfigureCompilers(collection, zone, zoneDefinition, searchPath, zoneStates);
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
