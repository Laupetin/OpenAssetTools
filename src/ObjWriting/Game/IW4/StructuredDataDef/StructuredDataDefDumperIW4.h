#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <cstddef>

namespace IW4::structured_data_def
{
    class Dumper final : public AbstractAssetDumper<StructuredDataDefSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
} // namespace IW4::structured_data_def
