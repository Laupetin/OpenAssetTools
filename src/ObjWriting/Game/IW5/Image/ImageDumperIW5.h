#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetImage>
    {
    public:
        explicit DumperIW5(const AssetPool<IW5::AssetImage::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
