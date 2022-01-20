#pragma once

#include <cstddef>
#include <vector>

#include "Dumping/AbstractAssetDumper.h"
#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperStructuredDataDefSet final : public AbstractAssetDumper<StructuredDataDefSet>
    {
        static bool GetNextHeadValue(const StructuredDataDef* def, bool isFirstStruct, const std::vector<bool>& structsIncludedInOrder, size_t& nextHeadValue);
        static StructuredDataType GetBaseType(const StructuredDataDef* def, StructuredDataType type);
        static std::vector<size_t> CalculateStructDumpingOrder(const StructuredDataDef* def);
        static void DumpEnum(StructuredDataDefDumper& dumper, int enumIndex, const StructuredDataEnum* _enum);
        static void DumpProperty(StructuredDataDefDumper& dumper, const StructuredDataStructProperty& property, const StructuredDataDef* def, int rootStructIndex);
        static void DumpStruct(StructuredDataDefDumper& dumper, size_t structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def, int rootStructIndex);

    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
}
