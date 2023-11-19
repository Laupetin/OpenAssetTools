#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

namespace IW4
{
    class AssetLoaderStructuredDataDefSet final : public BasicAssetLoader<ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        static StructuredDataType ConvertType(CommonStructuredDataType inputType);
        static void ConvertEnum(StructuredDataEnum* outputEnum, CommonStructuredDataEnum* inputEnum, MemoryManager* memory);
        static void ConvertStruct(StructuredDataStruct* outputStruct, CommonStructuredDataStruct* inputStruct, MemoryManager* memory);
        static void ConvertIndexedArray(StructuredDataIndexedArray* outputIndexedArray,
                                        const CommonStructuredDataIndexedArray* inputIndexedArray,
                                        MemoryManager* memory);
        static void
            ConvertEnumedArray(StructuredDataEnumedArray* outputEnumedArray, const CommonStructuredDataEnumedArray* inputEnumedArray, MemoryManager* memory);
        static void ConvertDef(StructuredDataDef* outputDef, const CommonStructuredDataDef* inputDef, MemoryManager* memory);
        static StructuredDataDefSet*
            ConvertSet(const std::string& assetName, const std::vector<std::unique_ptr<CommonStructuredDataDef>>& defs, MemoryManager* memory);
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace IW4
