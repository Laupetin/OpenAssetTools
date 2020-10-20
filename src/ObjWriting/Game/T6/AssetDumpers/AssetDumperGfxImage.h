#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Image/IImageWriter.h"

namespace T6
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<GfxImage>
    {
        IImageWriter* m_writer;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<GfxImage>* asset, FileAPI::File* out) override;

    public:
        AssetDumperGfxImage();
        ~AssetDumperGfxImage() override;

        AssetDumperGfxImage(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage(AssetDumperGfxImage&& other) noexcept = delete;
        AssetDumperGfxImage& operator=(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage& operator=(AssetDumperGfxImage&& other) noexcept = delete;
    };
}
