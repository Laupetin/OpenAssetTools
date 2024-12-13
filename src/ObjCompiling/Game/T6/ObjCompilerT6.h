#pragma once

#include "IAssetCompiler.h"
#include "IObjCompiler.h"

namespace T6
{
    class ObjCompiler final : public IObjCompiler
    {
    public:
        ObjCompilerResult CompileAssetForZone(AssetLoadingContext& context, asset_type_t assetType, const std::string& assetName) const override;

    private:
        std::unordered_map<asset_type_t, std::unique_ptr<IAssetCompiler>> m_asset_compilers_by_type;
    };
} // namespace T6
