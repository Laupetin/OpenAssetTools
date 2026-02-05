#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Image/ImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetImage>
    {
    public:
        DumperIW4();

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<ImageWriter> m_writer;
    };
} // namespace image
