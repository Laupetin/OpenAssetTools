#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::GfxImage>
    {
    public:
        DumperIW5();

    protected:
        bool ShouldDump(XAssetInfo<IW5::GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::GfxImage>* asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
