#pragma once

#include <cstddef>
#include <vector>

#include "Dumping/AbstractAssetDumper.h"
#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"
#include "Game/IW4/IW4.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

namespace IW4
{
    class AssetDumperStructuredDataDefSet final : public AbstractAssetDumper<StructuredDataDefSet>
    {
        static bool GetNextHeadValue(const StructuredDataDef* def, bool isFirstStruct, const std::vector<bool>& structsIncludedInOrder, size_t& nextHeadValue);
        static StructuredDataType GetBaseType(const StructuredDataDef* def, StructuredDataType type);
        static std::vector<size_t> CalculateStructDumpingOrder(const StructuredDataDef* def);
        static void DumpEnum(StructuredDataDefDumper& dumper, int enumIndex, const StructuredDataEnum* _enum);
        static size_t GetPropertySizeInBits(const StructuredDataStructProperty& property, const StructuredDataDef* def);
        static size_t GetPropertyAlign(const StructuredDataStructProperty& property);
        static void DumpProperty(StructuredDataDefDumper& dumper, const StructuredDataStructProperty& property, const StructuredDataDef* def, int rootStructIndex);
        static void DumpStruct(StructuredDataDefDumper& dumper, size_t structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def, int rootStructIndex);

        static CommonStructuredDataType ConvertType(const CommonStructuredDataDef* def, const StructuredDataType in);
        static void ConvertEnum(CommonStructuredDataEnum* out, const StructuredDataEnum* in, size_t enumIndex);
        static void ConvertStruct(const CommonStructuredDataDef* def, const StructuredDataDef* gameDef, CommonStructuredDataStruct* out, const StructuredDataStruct* in, const size_t structIndex);
        static void ConvertIndexedArray(const CommonStructuredDataDef* def, CommonStructuredDataIndexedArray* out, const StructuredDataIndexedArray* in);
        static void ConvertEnumedArray(const CommonStructuredDataDef* def, CommonStructuredDataEnumedArray* out, const StructuredDataEnumedArray* in);
        static std::unique_ptr<CommonStructuredDataDef> ConvertDef(const StructuredDataDef* in);
    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
}
