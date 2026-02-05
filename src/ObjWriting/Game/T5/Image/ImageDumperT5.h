#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"
#include "Image/ImageWriter.h"

#include <memory>

namespace image
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetImage>
    {
    public:
        DumperT5();

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<ImageWriter> m_writer;
    };
} // namespace image
