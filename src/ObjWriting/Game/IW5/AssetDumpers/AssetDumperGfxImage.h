#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace IW5
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<GfxImage>
    {
        std::unique_ptr<IImageWriter> m_writer;

        std::string GetAssetFileName(XAssetInfo<GfxImage>* asset) const;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset) override;

    public:
        AssetDumperGfxImage();
    };
} // namespace IW5
