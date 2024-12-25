#include "LoaderStructuredDataDefIW4.h"

#include "Game/IW4/IW4.h"
#include "StructuredDataDef/StructuredDataDefReader.h"
#include "Utils/Alignment.h"

#include <cassert>
#include <cstring>

using namespace IW4;

namespace
{
    class StructuredDataDefLoader final : public AssetCreator<AssetStructuredDataDef>
    {
    public:
        StructuredDataDefLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            StructuredDataDefReader reader(*file.m_stream,
                                           assetName,
                                           [this](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
                                           {
                                               auto foundFileToInclude = m_search_path.Open(filename);
                                               if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
                                                   return nullptr;

                                               return std::move(foundFileToInclude.m_stream);
                                           });

            bool readingDefsSuccessful;
            const auto defs = reader.ReadStructureDataDefs(readingDefsSuccessful);

            if (!readingDefsSuccessful)
                return AssetCreationResult::Failure();

            return AssetCreationResult::Success(context.AddAsset<AssetStructuredDataDef>(assetName, ConvertSet(assetName, defs)));
        }

    private:
        StructuredDataType ConvertType(CommonStructuredDataType inputType)
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

        void ConvertEnum(CommonStructuredDataEnum& inputEnum, StructuredDataEnum& outputEnum)
        {
            outputEnum.entryCount = static_cast<int>(inputEnum.m_entries.size());
            if (inputEnum.m_reserved_entry_count <= 0)
                outputEnum.reservedEntryCount = outputEnum.entryCount;
            else
                outputEnum.reservedEntryCount = inputEnum.m_reserved_entry_count;

            inputEnum.SortEntriesByName();
            if (!inputEnum.m_entries.empty())
            {
                outputEnum.entries = m_memory.Alloc<StructuredDataEnumEntry>(inputEnum.m_entries.size());
                for (auto entryIndex = 0u; entryIndex < inputEnum.m_entries.size(); entryIndex++)
                {
                    auto& outputEntry = outputEnum.entries[entryIndex];
                    const auto& inputEntry = inputEnum.m_entries[entryIndex];

                    outputEntry.string = m_memory.Dup(inputEntry.m_name.c_str());
                    outputEntry.index = static_cast<uint16_t>(inputEntry.m_value);
                }
            }
            else
                outputEnum.entries = nullptr;
        }

        void ConvertStruct(CommonStructuredDataStruct& inputStruct, StructuredDataStruct& outputStruct)
        {
            outputStruct.size = static_cast<int>(inputStruct.m_size_in_byte);
            outputStruct.bitOffset = inputStruct.m_bit_offset;

            outputStruct.propertyCount = static_cast<int>(inputStruct.m_properties.size());
            inputStruct.SortPropertiesByName();
            if (!inputStruct.m_properties.empty())
            {
                outputStruct.properties = m_memory.Alloc<StructuredDataStructProperty>(inputStruct.m_properties.size());
                for (auto propertyIndex = 0u; propertyIndex < inputStruct.m_properties.size(); propertyIndex++)
                {
                    auto& outputProperty = outputStruct.properties[propertyIndex];
                    const auto& inputProperty = inputStruct.m_properties[propertyIndex];

                    outputProperty.name = m_memory.Dup(inputProperty.m_name.c_str());
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
                outputStruct.properties = nullptr;
        }

        void ConvertIndexedArray(const CommonStructuredDataIndexedArray& inputIndexedArray, StructuredDataIndexedArray& outputIndexedArray)
        {
            outputIndexedArray.arraySize = static_cast<int>(inputIndexedArray.m_element_count);
            outputIndexedArray.elementType = ConvertType(inputIndexedArray.m_array_type);
            outputIndexedArray.elementSize = utils::Align(inputIndexedArray.m_element_size_in_bits, 8u) / 8u;
        }

        void ConvertEnumedArray(const CommonStructuredDataEnumedArray& inputEnumedArray, StructuredDataEnumedArray& outputEnumedArray)
        {
            outputEnumedArray.enumIndex = static_cast<int>(inputEnumedArray.m_enum_index);
            outputEnumedArray.elementType = ConvertType(inputEnumedArray.m_array_type);
            outputEnumedArray.elementSize = utils::Align(inputEnumedArray.m_element_size_in_bits, 8u) / 8u;
        }

        void ConvertDef(const CommonStructuredDataDef& inputDef, StructuredDataDef& outputDef)
        {
            outputDef.version = inputDef.m_version;
            outputDef.formatChecksum = inputDef.m_checksum;

            outputDef.enumCount = static_cast<int>(inputDef.m_enums.size());
            if (!inputDef.m_enums.empty())
            {
                outputDef.enums = m_memory.Alloc<StructuredDataEnum>(inputDef.m_enums.size());
                for (auto enumIndex = 0u; enumIndex < inputDef.m_enums.size(); enumIndex++)
                    ConvertEnum(*inputDef.m_enums[enumIndex], outputDef.enums[enumIndex]);
            }
            else
                outputDef.enums = nullptr;

            outputDef.structCount = static_cast<int>(inputDef.m_structs.size());
            if (!inputDef.m_structs.empty())
            {
                outputDef.structs = m_memory.Alloc<StructuredDataStruct>(inputDef.m_structs.size());
                for (auto structIndex = 0u; structIndex < inputDef.m_structs.size(); structIndex++)
                    ConvertStruct(*inputDef.m_structs[structIndex], outputDef.structs[structIndex]);
            }
            else
                outputDef.structs = nullptr;

            outputDef.indexedArrayCount = static_cast<int>(inputDef.m_indexed_arrays.size());
            if (!inputDef.m_indexed_arrays.empty())
            {
                outputDef.indexedArrays = m_memory.Alloc<StructuredDataIndexedArray>(inputDef.m_indexed_arrays.size());
                for (auto indexedArrayIndex = 0u; indexedArrayIndex < inputDef.m_indexed_arrays.size(); indexedArrayIndex++)
                    ConvertIndexedArray(inputDef.m_indexed_arrays[indexedArrayIndex], outputDef.indexedArrays[indexedArrayIndex]);
            }
            else
                outputDef.indexedArrays = nullptr;

            outputDef.enumedArrayCount = static_cast<int>(inputDef.m_enumed_arrays.size());
            if (!inputDef.m_enumed_arrays.empty())
            {
                outputDef.enumedArrays = m_memory.Alloc<StructuredDataEnumedArray>(inputDef.m_enumed_arrays.size());
                for (auto enumedArrayIndex = 0u; enumedArrayIndex < inputDef.m_enumed_arrays.size(); enumedArrayIndex++)
                    ConvertEnumedArray(inputDef.m_enumed_arrays[enumedArrayIndex], outputDef.enumedArrays[enumedArrayIndex]);
            }
            else
                outputDef.enumedArrays = nullptr;

            outputDef.rootType = ConvertType(inputDef.m_root_type);
            outputDef.size = inputDef.m_size_in_byte;
        }

        StructuredDataDefSet* ConvertSet(const std::string& assetName, const std::vector<std::unique_ptr<CommonStructuredDataDef>>& commonDefs)
        {
            auto* set = m_memory.Alloc<StructuredDataDefSet>();
            set->name = m_memory.Dup(assetName.c_str());

            set->defCount = commonDefs.size();
            set->defs = m_memory.Alloc<StructuredDataDef>(commonDefs.size());

            for (auto defIndex = 0u; defIndex < commonDefs.size(); defIndex++)
                ConvertDef(*commonDefs[defIndex], set->defs[defIndex]);

            return set;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetStructuredDataDef>> CreateStructuredDataDefLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<StructuredDataDefLoader>(memory, searchPath);
    }
} // namespace IW4
