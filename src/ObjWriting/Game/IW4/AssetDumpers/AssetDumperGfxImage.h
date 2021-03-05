#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"
#include "Image/IImageWriter.h"

namespace IW4
{
    class AssetDumperGfxImage final : public AbstractFileDumper<GfxImage>
    {
        IImageWriter* m_writer;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset, std::ostream& stream) override;

    public:
        AssetDumperGfxImage();
        ~AssetDumperGfxImage() override;

        AssetDumperGfxImage(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage(AssetDumperGfxImage&& other) noexcept = delete;
        AssetDumperGfxImage& operator=(const AssetDumperGfxImage& other) = delete;
        AssetDumperGfxImage& operator=(AssetDumperGfxImage&& other) noexcept = delete;
    };
}
