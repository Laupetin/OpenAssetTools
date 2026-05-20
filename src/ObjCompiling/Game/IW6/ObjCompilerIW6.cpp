#include "ObjCompilerIW6.h"

#include "Game/IW6/IW6.h"
#include "Game/IW6/Techset/TechniqueCompilerIW6.h"
#include "Game/IW6/Techset/TechsetCompilerIW6.h"
#include "Game/IW6/Techset/VertexDeclCompilerIW6.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW6;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = zone.Memory();

        collection.AddAssetCreator(techset::CreateVertexDeclCompilerIW6(memory));
        collection.AddAssetCreator(techset::CreateTechsetCompilerIW6(memory, searchPath));

        collection.AddSubAssetCreator(techset::CreateTechniqueCompilerIW6(memory, zone, searchPath));
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
    ConfigureCompilers(collection, zone, searchPath);
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
