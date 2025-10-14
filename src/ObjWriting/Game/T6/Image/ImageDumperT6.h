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
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::GfxImage>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
