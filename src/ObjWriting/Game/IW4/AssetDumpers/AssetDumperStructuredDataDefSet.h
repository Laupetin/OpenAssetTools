#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperStructuredDataDefSet final : public AbstractAssetDumper<StructuredDataDefSet>
    {
        static void DumpEnum(StructuredDataDefDumper& dumper, int enumIndex, const StructuredDataEnum* _enum);
        static void DumpProperty(StructuredDataDefDumper& dumper, const StructuredDataStructProperty& property, const StructuredDataDef* def, int rootStructIndex);
        static void DumpStruct(StructuredDataDefDumper& dumper, int structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def, bool isRoot);

    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
}
