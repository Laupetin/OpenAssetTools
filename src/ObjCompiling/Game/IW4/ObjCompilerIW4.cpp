#include "ObjCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Image/ImageIwdPostProcessor.h"
#include "Material/CompilerMaterialIW4.h"
#include "Techset/CompilerTechsetIW4.h"
#include "Techset/CompilerVertexDeclIW4.h"

#include <memory>

using namespace IW4;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt)
    {
        auto& memory = zone.Memory();

#ifdef EXPERIMENTAL_MATERIAL_COMPILATION
        collection.AddAssetCreator(CreateMaterialCompiler(memory, searchPath, gdt));
        collection.AddAssetCreator(CreateTechsetLoader(memory, searchPath));
#endif
        collection.AddAssetCreator(CreateVertexDeclLoader(memory));
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection,
                                 Zone& zone,
                                 const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 IOutputPath& outDir)
    {
        auto& memory = zone.Memory();

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
    ConfigureCompilers(collection, zone, searchPath, gdt);
    ConfigurePostProcessors(collection, zone, zoneDefinition, searchPath, zoneStates, outDir);
}
