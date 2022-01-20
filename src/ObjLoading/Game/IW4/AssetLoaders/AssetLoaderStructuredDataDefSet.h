#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "Parsing/StructuredDataDef/Domain/CommonStructuredDataDef.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderStructuredDataDefSet final : public BasicAssetLoader<ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDef>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        static StructuredDataType ConvertType(CommonStructuredDataDefType inputType);
        static void ConvertEnum(StructuredDataEnum* outputEnum, const CommonStructuredDataDefEnum* inputEnum, MemoryManager* memory);
        static void ConvertStruct(StructuredDataStruct* outputStruct, const CommonStructuredDataDefStruct* inputStruct, MemoryManager* memory);
        static void ConvertIndexedArray(StructuredDataIndexedArray* outputIndexedArray, const CommonStructuredDataDefIndexedArray* inputIndexedArray, MemoryManager* memory);
        static void ConvertEnumedArray(StructuredDataEnumedArray* outputEnumedArray, const CommonStructuredDataDefEnumedArray* inputEnumedArray, MemoryManager* memory);
        static void ConvertDef(StructuredDataDef* outputDef, const CommonStructuredDataDef* inputDef, MemoryManager* memory);
        static StructuredDataDefSet* ConvertSet(const std::string& assetName, const std::vector<std::unique_ptr<CommonStructuredDataDef>>& defs, MemoryManager* memory);
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
