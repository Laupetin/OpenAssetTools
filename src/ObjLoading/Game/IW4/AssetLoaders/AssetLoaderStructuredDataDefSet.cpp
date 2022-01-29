#include "AssetLoaderStructuredDataDefSet.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Parsing/StructuredDataDef/StructuredDataDefReader.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderStructuredDataDefSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* structuredDataDefSet = memory->Create<StructuredDataDefSet>();
    memset(structuredDataDefSet, 0, sizeof(StructuredDataDefSet));
    structuredDataDefSet->name = memory->Dup(assetName.c_str());
    return structuredDataDefSet;
}

bool AssetLoaderStructuredDataDefSet::CanLoadFromRaw() const
{
    return true;
}

StructuredDataType AssetLoaderStructuredDataDefSet::ConvertType(CommonStructuredDataType inputType)
{
    switch (inputType.m_category)
    {
    case CommonStructuredDataTypeCategory::INT:
        return {DATA_INT, {0}};
    case CommonStructuredDataTypeCategory::BYTE:
        return {DATA_BYTE, {0}};
    case CommonStructuredDataTypeCategory::BOOL:
        return {DATA_BOOL, {0}};
    case CommonStructuredDataTypeCategory::FLOAT:
        return {DATA_FLOAT, {0}};
    case CommonStructuredDataTypeCategory::SHORT:
        return {DATA_SHORT, {0}};
    case CommonStructuredDataTypeCategory::STRING:
        return {DATA_STRING, {inputType.m_info.string_length}};
    case CommonStructuredDataTypeCategory::ENUM:
        return {DATA_ENUM, {inputType.m_info.type_index}};
    case CommonStructuredDataTypeCategory::STRUCT:
        return {DATA_STRUCT, {inputType.m_info.type_index}};
    case CommonStructuredDataTypeCategory::INDEXED_ARRAY:
        return {DATA_INDEXED_ARRAY, {inputType.m_info.type_index}};
    case CommonStructuredDataTypeCategory::ENUM_ARRAY:
        return {DATA_ENUM_ARRAY, {inputType.m_info.type_index}};
    case CommonStructuredDataTypeCategory::UNKNOWN:
    default:
        assert(false);
        return {DATA_INT, {0}};
    }
}

void AssetLoaderStructuredDataDefSet::ConvertEnum(StructuredDataEnum* outputEnum, const CommonStructuredDataEnum* inputEnum, MemoryManager* memory)
{
    outputEnum->entryCount = static_cast<int>(inputEnum->m_entries.size());
    if (inputEnum->m_reserved_entry_count <= 0)
        outputEnum->reservedEntryCount = outputEnum->entryCount;
    else
        outputEnum->reservedEntryCount = inputEnum->m_reserved_entry_count;

    if (!inputEnum->m_entries.empty())
    {
        outputEnum->entries = static_cast<StructuredDataEnumEntry*>(memory->Alloc(sizeof(StructuredDataEnumEntry) * inputEnum->m_entries.size()));
        for (auto entryIndex = 0u; entryIndex < inputEnum->m_entries.size(); entryIndex++)
        {
            auto& outputEntry = outputEnum->entries[entryIndex];
            const auto& inputEntry = inputEnum->m_entries[entryIndex];

            outputEntry.string = memory->Dup(inputEntry.m_name.c_str());
            outputEntry.index = static_cast<uint16_t>(inputEntry.m_value);
        }
    }
    else
        outputEnum->entries = nullptr;
}

void AssetLoaderStructuredDataDefSet::ConvertStruct(StructuredDataStruct* outputStruct, const CommonStructuredDataStruct* inputStruct, MemoryManager* memory)
{
    outputStruct->size = static_cast<int>(inputStruct->m_size_in_byte);
    outputStruct->bitOffset = inputStruct->m_bit_offset;

    outputStruct->propertyCount = static_cast<int>(inputStruct->m_properties.size());
    if (!inputStruct->m_properties.empty())
    {
        outputStruct->properties = static_cast<StructuredDataStructProperty*>(memory->Alloc(sizeof(StructuredDataStructProperty) * inputStruct->m_properties.size()));
        for (auto propertyIndex = 0u; propertyIndex < inputStruct->m_properties.size(); propertyIndex++)
        {
            auto& outputProperty = outputStruct->properties[propertyIndex];
            const auto& inputProperty = inputStruct->m_properties[propertyIndex];

            outputProperty.name = memory->Dup(inputProperty.m_name.c_str());
            outputProperty.type = ConvertType(inputProperty.m_type);

            if (outputProperty.type.type != DATA_BOOL)
            {
                assert(inputProperty.m_offset_in_bits % 8 == 0);
                outputProperty.offset = inputProperty.m_offset_in_bits / 8;
            }
            else
                outputProperty.offset = inputProperty.m_offset_in_bits;
        }
    }
    else
        outputStruct->properties = nullptr;
}

void AssetLoaderStructuredDataDefSet::ConvertIndexedArray(StructuredDataIndexedArray* outputIndexedArray, const CommonStructuredDataIndexedArray* inputIndexedArray, MemoryManager* memory)
{
    outputIndexedArray->arraySize = static_cast<int>(inputIndexedArray->m_element_count);
    outputIndexedArray->elementType = ConvertType(inputIndexedArray->m_array_type);
    outputIndexedArray->elementSize = inputIndexedArray->m_element_size_in_bits;
}

void AssetLoaderStructuredDataDefSet::ConvertEnumedArray(StructuredDataEnumedArray* outputEnumedArray, const CommonStructuredDataEnumedArray* inputEnumedArray, MemoryManager* memory)
{
    outputEnumedArray->enumIndex = static_cast<int>(inputEnumedArray->m_enum_index);
    outputEnumedArray->elementType = ConvertType(inputEnumedArray->m_array_type);
    outputEnumedArray->elementSize = inputEnumedArray->m_element_size_in_bits;
}

void AssetLoaderStructuredDataDefSet::ConvertDef(StructuredDataDef* outputDef, const CommonStructuredDataDef* inputDef, MemoryManager* memory)
{
    outputDef->version = inputDef->m_version;
    outputDef->formatChecksum = inputDef->m_checksum;

    outputDef->enumCount = static_cast<int>(inputDef->m_enums.size());
    if (!inputDef->m_enums.empty())
    {
        outputDef->enums = static_cast<StructuredDataEnum*>(memory->Alloc(sizeof(StructuredDataEnum) * inputDef->m_enums.size()));
        for (auto enumIndex = 0u; enumIndex < inputDef->m_enums.size(); enumIndex++)
            ConvertEnum(&outputDef->enums[enumIndex], inputDef->m_enums[enumIndex].get(), memory);
    }
    else
        outputDef->enums = nullptr;

    outputDef->structCount = static_cast<int>(inputDef->m_structs.size());
    if (!inputDef->m_structs.empty())
    {
        outputDef->structs = static_cast<StructuredDataStruct*>(memory->Alloc(sizeof(StructuredDataStruct) * inputDef->m_structs.size()));
        for (auto structIndex = 0u; structIndex < inputDef->m_structs.size(); structIndex++)
            ConvertStruct(&outputDef->structs[structIndex], inputDef->m_structs[structIndex].get(), memory);
    }
    else
        outputDef->structs = nullptr;

    outputDef->indexedArrayCount = static_cast<int>(inputDef->m_indexed_arrays.size());
    if (!inputDef->m_indexed_arrays.empty())
    {
        outputDef->indexedArrays = static_cast<StructuredDataIndexedArray*>(memory->Alloc(sizeof(StructuredDataIndexedArray) * inputDef->m_indexed_arrays.size()));
        for (auto indexedArrayIndex = 0u; indexedArrayIndex < inputDef->m_indexed_arrays.size(); indexedArrayIndex++)
            ConvertIndexedArray(&outputDef->indexedArrays[indexedArrayIndex], &inputDef->m_indexed_arrays[indexedArrayIndex], memory);
    }
    else
        outputDef->indexedArrays = nullptr;

    outputDef->enumedArrayCount = static_cast<int>(inputDef->m_enumed_arrays.size());
    if (!inputDef->m_enumed_arrays.empty())
    {
        outputDef->enumedArrays = static_cast<StructuredDataEnumedArray*>(memory->Alloc(sizeof(StructuredDataEnumedArray) * inputDef->m_enumed_arrays.size()));
        for (auto enumedArrayIndex = 0u; enumedArrayIndex < inputDef->m_enumed_arrays.size(); enumedArrayIndex++)
            ConvertEnumedArray(&outputDef->enumedArrays[enumedArrayIndex], &inputDef->m_enumed_arrays[enumedArrayIndex], memory);
    }
    else
        outputDef->enumedArrays = nullptr;

    outputDef->rootType = ConvertType(inputDef->m_root_type);
    outputDef->size = inputDef->m_size_in_byte;
}

StructuredDataDefSet* AssetLoaderStructuredDataDefSet::ConvertSet(const std::string& assetName, const std::vector<std::unique_ptr<CommonStructuredDataDef>>& defs, MemoryManager* memory)
{
    auto* set = memory->Create<StructuredDataDefSet>();
    set->name = memory->Dup(assetName.c_str());
    set->defCount = defs.size();
    set->defs = static_cast<StructuredDataDef*>(memory->Alloc(sizeof(StructuredDataDef) * defs.size()));

    for (auto defIndex = 0u; defIndex < defs.size(); defIndex++)
        ConvertDef(&set->defs[defIndex], defs[defIndex].get(), memory);

    return set;
}

bool AssetLoaderStructuredDataDefSet::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    StructuredDataDefReader reader(*file.m_stream, assetName, [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
    {
        auto foundFileToInclude = searchPath->Open(filename);
        if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
            return nullptr;

        return std::move(foundFileToInclude.m_stream);
    });

    bool readingDefsSuccessful;
    const auto defs = reader.ReadStructureDataDefs(readingDefsSuccessful);

    if (readingDefsSuccessful)
        manager->AddAsset(ASSET_TYPE_STRUCTURED_DATA_DEF, assetName, ConvertSet(assetName, defs, memory));

    return true;
}
