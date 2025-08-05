#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperT5 final : public AbstractAssetDumper<T5::GfxImage>
    {
    public:
        DumperT5();

    protected:
        bool ShouldDump(XAssetInfo<T5::GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T5::GfxImage>* asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
