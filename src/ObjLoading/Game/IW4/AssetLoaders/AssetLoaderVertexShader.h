#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderVertexShader final : public BasicAssetLoader<ASSET_TYPE_VERTEXSHADER, MaterialVertexShader>
    {
        static constexpr auto MAX_SHADER_SIZE = 0x1900000u;

    public:
        _NODISCARD static std::string GetFileNameForSourceShader(const std::string& assetName);
        static bool LoadFromSource(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone);

        _NODISCARD static std::string GetFileNameForCompiledShader(const std::string& assetName);
        static bool LoadCompiled(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone);

        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
