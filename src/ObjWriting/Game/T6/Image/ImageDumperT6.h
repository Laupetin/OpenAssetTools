#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperT6 final : public AbstractAssetDumper<T6::GfxImage>
    {
    public:
        DumperT6();

    protected:
        bool ShouldDump(XAssetInfo<T6::GfxImage>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::GfxImage>* asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
