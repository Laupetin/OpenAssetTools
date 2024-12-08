#pragma once

#include "AssetLoading/AssetLoadingContext.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

#include <cstdint>
#include <string>

enum class ObjCompilerResult : std::uint8_t
{
    COMPILED,
    NO_COMPILATION_DONE,
    FAILURE
};

class IObjCompiler
{
public:
    IObjCompiler() = default;
    virtual ~IObjCompiler() = default;
    IObjCompiler(const IObjCompiler& other) = default;
    IObjCompiler(IObjCompiler&& other) noexcept = default;
    IObjCompiler& operator=(const IObjCompiler& other) = default;
    IObjCompiler& operator=(IObjCompiler&& other) noexcept = default;

    virtual ObjCompilerResult CompileAssetForZone(AssetLoadingContext& context, asset_type_t assetType, const std::string& assetName) const = 0;

    static const IObjCompiler* GetObjCompilerForGame(GameId game);
};
