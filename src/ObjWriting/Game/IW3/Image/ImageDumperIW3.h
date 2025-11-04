#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetImage>
    {
    public:
        explicit DumperIW3(const AssetPool<IW3::AssetImage::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
