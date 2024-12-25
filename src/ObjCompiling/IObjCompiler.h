#pragma once

#include "Asset/AssetCreatorCollection.h"
#include "AssetLoading/AssetLoadingContext.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "Zone/Zone.h"

#include <cstdint>
#include <string>

class IObjCompiler
{
public:
    IObjCompiler() = default;
    virtual ~IObjCompiler() = default;
    IObjCompiler(const IObjCompiler& other) = default;
    IObjCompiler(IObjCompiler&& other) noexcept = default;
    IObjCompiler& operator=(const IObjCompiler& other) = default;
    IObjCompiler& operator=(IObjCompiler&& other) noexcept = default;

    virtual void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, const ZoneDefinition& zoneDefinition) const = 0;

    static const IObjCompiler* GetObjCompilerForGame(GameId game);
};