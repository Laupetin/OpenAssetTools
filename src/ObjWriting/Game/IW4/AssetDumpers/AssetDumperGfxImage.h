#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Image/IImageWriter.h"

namespace IW4
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<IW4::GfxImage>
    {
        IImageWriter* m_writer;

    protected:
        bool ShouldDump(IW4::GfxImage* asset) override;
        std::string GetFileNameForAsset(Zone* zone, IW4::GfxImage* asset) override;
        void DumpAsset(Zone* zone, IW4::GfxImage* asset, FileAPI::File* out) override;

    public:
        AssetDumperGfxImage();
        ~AssetDumperGfxImage();

        AssetDumperGfxImage(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage(AssetDumperGfxImage&& other) noexcept = delete;
        AssetDumperGfxImage& operator=(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage& operator=(AssetDumperGfxImage&& other) noexcept = delete;
    };
}
