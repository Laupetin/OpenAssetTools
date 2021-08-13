#pragma once

#include <memory>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"
#include "Image/IImageWriter.h"

namespace T5
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
}