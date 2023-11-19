#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <cstddef>

namespace IW4
{
    class AssetDumperStructuredDataDefSet final : public AbstractAssetDumper<StructuredDataDefSet>
    {
        static CommonStructuredDataType ConvertType(const CommonStructuredDataDef* def, StructuredDataType in);
        static void ConvertEnum(CommonStructuredDataEnum* out, const StructuredDataEnum* in, size_t enumIndex);
        static void ConvertStruct(const CommonStructuredDataDef* def,
                                  const StructuredDataDef* gameDef,
                                  CommonStructuredDataStruct* out,
                                  const StructuredDataStruct* in,
                                  size_t structIndex);
        static void ConvertIndexedArray(const CommonStructuredDataDef* def, CommonStructuredDataIndexedArray* out, const StructuredDataIndexedArray* in);
        static void ConvertEnumedArray(const CommonStructuredDataDef* def, CommonStructuredDataEnumedArray* out, const StructuredDataEnumedArray* in);
        static std::unique_ptr<CommonStructuredDataDef> ConvertDef(const StructuredDataDef* in);

    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
} // namespace IW4
