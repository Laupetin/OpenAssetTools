#include "ObjCompilerQOS.h"

using namespace QOS;

void ObjCompiler::ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                             Zone& zone,
                                             const ZoneDefinitionContext& zoneDefinition,
                                             ISearchPath& searchPath,
                                             IGdtQueryable& gdt,
                                             ZoneAssetCreationStateContainer& zoneStates,
                                             IOutputPath& outDir,
                                             IOutputPath& cacheDir) const
{
    (void)collection;
    (void)zone;
    (void)zoneDefinition;
    (void)searchPath;
    (void)gdt;
    (void)zoneStates;
    (void)outDir;
    (void)cacheDir;
}
