#pragma once

#include <memory>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Image/IImageWriter.h"

namespace IW3
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<GfxImage>
    {
        std::unique_ptr<IImageWriter> m_writer;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        bool CanDumpAsRaw() override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset, std::ostream& stream) override;

    public:
        AssetDumperGfxImage();
    };
}
