#include "ObjCompilerIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/Techset/TechniqueCompilerIW5.h"
#include "Game/IW5/Techset/TechsetCompilerIW5.h"
#include "Game/IW5/Techset/VertexDeclCompilerIW5.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW5;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = zone.Memory();

        collection.AddAssetCreator(techset::CreateVertexDeclCompilerIW5(memory));
        collection.AddAssetCreator(techset::CreateTechsetCompilerIW5(memory, searchPath));

        collection.AddSubAssetCreator(techset::CreateTechniqueCompilerIW5(memory, zone, searchPath));
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
