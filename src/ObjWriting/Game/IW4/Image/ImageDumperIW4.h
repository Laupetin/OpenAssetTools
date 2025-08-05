#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::GfxImage>
    {
    public:
        DumperIW4();

    protected:
        bool ShouldDump(XAssetInfo<IW4::GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::GfxImage>* asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
