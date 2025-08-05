#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <cstddef>

namespace structured_data_def
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::StructuredDataDefSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::StructuredDataDefSet>* asset) override;
    };
} // namespace structured_data_def
