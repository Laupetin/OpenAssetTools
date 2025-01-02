#include "ObjCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Image/ImageIwdPostProcessor.h"

#include <memory>

using namespace IW4;
namespace fs = std::filesystem;

namespace
{
    void ConfigureCompilers(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = *zone.GetMemory();

        // No compilers yet
    }

    void ConfigurePostProcessors(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, const fs::path& outDir)
    {
        auto& memory = *zone.GetMemory();

        collection.AddAssetPostProcessor(std::make_unique<ImageIwdPostProcessor<AssetImage>>(searchPath, outDir));
    }
} // namespace

void ObjCompiler::ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                             Zone& zone,
                                             const ZoneDefinition& zoneDefinition,
                                             ISearchPath& searchPath,
                                             IGdtQueryable& gdt,
                                             const fs::path& outDir,
                                             const fs::path& cacheDir) const
{
    ConfigurePostProcessors(collection, zone, searchPath, outDir);
}
