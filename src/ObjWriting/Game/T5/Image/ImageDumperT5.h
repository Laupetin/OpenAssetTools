#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetImage>
    {
    public:
        explicit DumperT5(const AssetPool<T5::AssetImage::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
