#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetImage>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetImage::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
