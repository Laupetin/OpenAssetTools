#pragma once

#include "IObjCompiler.h"

namespace T5
{
    class ObjCompiler final : public IObjCompiler
    {
    public:
        ObjCompilerResult CompileAssetForZone(AssetLoadingContext& context, asset_type_t assetType, const std::string& assetName) const override;
    };
} // namespace T5
