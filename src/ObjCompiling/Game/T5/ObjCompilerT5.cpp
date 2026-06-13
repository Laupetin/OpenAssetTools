#include "ObjCompilerT5.h"

#include "Game/T5/T5.h"
#include "Game/T5/Techset/PixelShaderCompilerT5.h"
#include "Game/T5/Techset/TechniqueCompilerT5.h"
#include "Game/T5/Techset/TechsetCompilerT5.h"
#include "Game/T5/Techset/VertexDeclCompilerT5.h"
#include "Game/T5/Techset/VertexShaderCompilerT5.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace T5;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = zone.Memory();

        collection.AddAssetCreator(techset::CreateTechsetCompilerT5(memory, searchPath));

        collection.AddSubAssetCreator(techset::CreateTechniqueCompilerT5(memory, zone, searchPath));
        collection.AddSubAssetCreator(techset::CreateVertexDeclCompilerT5(memory));
        collection.AddSubAssetCreator(techset::CreateVertexShaderCompilerT5(memory, searchPath));
        collection.AddSubAssetCreator(techset::CreatePixelShaderCompilerT5(memory, searchPath));
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
