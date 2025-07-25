#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace IW3
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<GfxImage>
    {
        std::unique_ptr<IImageWriter> m_writer;

        [[nodiscard]] std::string GetAssetFileName(const XAssetInfo<GfxImage>& asset) const;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset) override;

    public:
        AssetDumperGfxImage();
    };
} // namespace IW3
