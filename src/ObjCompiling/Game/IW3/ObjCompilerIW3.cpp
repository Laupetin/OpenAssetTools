#include "ObjCompilerIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/Techset/PixelShaderCompilerIW3.h"
#include "Game/IW3/Techset/TechniqueCompilerIW3.h"
#include "Game/IW3/Techset/TechsetCompilerIW3.h"
#include "Game/IW3/Techset/VertexDeclCompilerIW3.h"
#include "Game/IW3/Techset/VertexShaderCompilerIW3.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW3;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = zone.Memory();

        collection.AddAssetCreator(techset::CreateTechsetCompilerIW3(memory, searchPath));

        collection.AddSubAssetCreator(techset::CreateTechniqueCompilerIW3(memory, zone, searchPath));
        collection.AddSubAssetCreator(techset::CreateVertexDeclCompilerIW3(memory));
        collection.AddSubAssetCreator(techset::CreateVertexShaderCompilerIW3(memory, searchPath));
        collection.AddSubAssetCreator(techset::CreatePixelShaderCompilerIW3(memory, searchPath));
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
