#pragma once

#include "Asset/AssetCreatorCollection.h"
#include "Asset/IZoneAssetCreationState.h"
#include "Asset/ZoneDefinitionContext.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/IOutputPath.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

class IObjCompiler
{
public:
    IObjCompiler() = default;
    virtual ~IObjCompiler() = default;
    IObjCompiler(const IObjCompiler& other) = default;
    IObjCompiler(IObjCompiler&& other) noexcept = default;
    IObjCompiler& operator=(const IObjCompiler& other) = default;
    IObjCompiler& operator=(IObjCompiler&& other) noexcept = default;

    virtual void ConfigureCreatorCollection(AssetCreatorCollection& collection,
                                            Zone& zone,
                                            const ZoneDefinitionContext& zoneDefinition,
                                            ISearchPath& searchPath,
                                            IGdtQueryable& gdt,
                                            ZoneAssetCreationStateContainer& zoneStates,
                                            IOutputPath& outDir,
                                            IOutputPath& cacheDir) const = 0;

    static const IObjCompiler* GetObjCompilerForGame(GameId game);
};
