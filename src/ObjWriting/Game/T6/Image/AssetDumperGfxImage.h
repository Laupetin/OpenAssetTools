#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace T6
{
    class AssetDumperGfxImage final : public AbstractAssetDumper<GfxImage>
    {
        std::unique_ptr<IImageWriter> m_writer;

        std::string GetAssetFileName(const XAssetInfo<GfxImage>& asset) const;

    protected:
        bool ShouldDump(XAssetInfo<GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset) override;

    public:
        AssetDumperGfxImage();
    };
} // namespace T6
