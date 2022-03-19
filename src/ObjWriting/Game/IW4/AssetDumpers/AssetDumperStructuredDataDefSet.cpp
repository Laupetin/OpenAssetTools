#include "AssetDumperStructuredDataDefSet.h"

#include <cassert>
#include <sstream>
#include <algorithm>

#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"
#include "StructuredDataDef/StructuredDataDefDumper.h"

using namespace IW4;
using namespace std::string_literals;

bool AssetDumperStructuredDataDefSet::GetNextHeadValue(const StructuredDataDef* def, const bool isFirstStruct, const std::vector<bool>& structsIncludedInOrder, size_t& nextHeadValue)
{
    if (isFirstStruct
        && def->rootType.type == DATA_STRUCT
        && def->rootType.u.structIndex >= 0
        && def->rootType.u.structIndex < def->structCount)
    {
        nextHeadValue = def->rootType.u.structIndex;
        return true;
    }

    const auto firstNotIncludedStruct = std::find(structsIncludedInOrder.begin(), structsIncludedInOrder.end(), false);
    assert(firstNotIncludedStruct != structsIncludedInOrder.end());

    if (firstNotIncludedStruct == structsIncludedInOrder.end())
        return false;

    nextHeadValue = static_cast<size_t>(firstNotIncludedStruct - structsIncludedInOrder.begin());
    return false;
}

StructuredDataType AssetDumperStructuredDataDefSet::GetBaseType(const StructuredDataDef* def, StructuredDataType type)
{
    while (true)
    {
        if (type.type == DATA_INDEXED_ARRAY)
        {
            if (def->indexedArrays != nullptr && type.u.indexedArrayIndex >= 0 && type.u.indexedArrayIndex < def->indexedArrayCount)
                type = def->indexedArrays[type.u.indexedArrayIndex].elementType;
            else
                break;
        }
        else if (type.type == DATA_ENUM_ARRAY)
        {
            if (def->enumedArrays != nullptr && type.u.enumedArrayIndex >= 0 && type.u.enumedArrayIndex < def->enumedArrayCount)
                type = def->enumedArrays[type.u.enumedArrayIndex].elementType;
            else
                break;
        }
        else
            break;
    }

    return type;
}

std::vector<size_t> AssetDumperStructuredDataDefSet::CalculateStructDumpingOrder(const StructuredDataDef* def)
{
    if (def->structCount <= 0 || def->structs == nullptr)
        return {};

    const auto structCount = static_cast<size_t>(def->structCount);

    std::vector<size_t> result;
    auto resultStructHead = 0u;
    auto resultStructTail = 0u;

    result.reserve(def->structCount);
    std::vector<bool> structIncludedInOrder(def->structCount);

    while (resultStructTail < structCount)
    {
        size_t nextHeadValue;
        if (!GetNextHeadValue(def, resultStructHead == 0, structIncludedInOrder, nextHeadValue))
            return result;
        result.push_back(nextHeadValue);
        structIncludedInOrder[nextHeadValue] = true;
        ++resultStructHead;

        while (resultStructHead > resultStructTail)
        {
            const auto& currentStruct = def->structs[result[resultStructTail++]];

            if (currentStruct.properties == nullptr)
                continue;

            for (auto i = 0; i < currentStruct.propertyCount; i++)
            {
                const auto baseType = GetBaseType(def, currentStruct.properties[i].type);
                if (baseType.type == DATA_STRUCT
                    && baseType.u.structIndex >= 0
                    && static_cast<size_t>(baseType.u.structIndex) < structCount
                    && structIncludedInOrder[static_cast<size_t>(baseType.u.structIndex)] == false)
                {
                    result.push_back(static_cast<size_t>(baseType.u.structIndex));
                    structIncludedInOrder[static_cast<size_t>(baseType.u.structIndex)] = true;
                    ++resultStructHead;
                }
            }
        }
    }

    return result;
}

bool AssetDumperStructuredDataDefSet::ShouldDump(XAssetInfo<StructuredDataDefSet>* asset)
{
    return true;
}

void AssetDumperStructuredDataDefSet::DumpEnum(StructuredDataDefDumper& dumper, const int enumIndex, const StructuredDataEnum* _enum)
{
    std::ostringstream ss;
    ss << "ENUM_" << enumIndex;

    dumper.BeginEnum(ss.str(), static_cast<size_t>(_enum->entryCount), static_cast<size_t>(_enum->reservedEntryCount));

    if (_enum->entries && _enum->entryCount > 0)
    {
        for (auto i = 0; i < _enum->entryCount; i++)
        {
            const auto& entry = _enum->entries[i];

            assert(entry.string);
            if (!entry.string)
                continue;

            dumper.WriteEnumEntry(entry.string, entry.index);
        }
    }

    dumper.EndEnum();
}

size_t AssetDumperStructuredDataDefSet::GetPropertySizeInBits(const StructuredDataStructProperty& property, const StructuredDataDef* def)
{
    switch (property.type.type)
    {
    case DATA_FLOAT:
    case DATA_INT:
        return 32;
    case DATA_BYTE:
        return 8;
    case DATA_BOOL:
        return 1;
    case DATA_SHORT:
    case DATA_ENUM:
        return 16;
    case DATA_STRING:
        return property.type.u.stringDataLength * 8;
    case DATA_STRUCT:
        if (property.type.u.structIndex < 0 || property.type.u.structIndex >= def->structCount || def->structs == nullptr)
        {
            assert(false);
            return 0;
        }
        return static_cast<size_t>(def->structs[property.type.u.structIndex].size) * 8;
    case DATA_INDEXED_ARRAY:
        {
            if (property.type.u.indexedArrayIndex < 0 || property.type.u.indexedArrayIndex >= def->indexedArrayCount || def->indexedArrays == nullptr)
            {
                assert(false);
                return 0;
            }
            const auto& indexedArray = def->indexedArrays[property.type.u.indexedArrayIndex];
            const auto elementSize = indexedArray.elementType.type == DATA_BOOL ? 1 : indexedArray.elementSize * 8;
            return elementSize * static_cast<size_t>(def->indexedArrays[property.type.u.indexedArrayIndex].arraySize);
        }
    case DATA_ENUM_ARRAY:
        {
            if (property.type.u.enumedArrayIndex < 0 || property.type.u.enumedArrayIndex >= def->enumedArrayCount || def->enumedArrays == nullptr)
            {
                assert(false);
                return 0;
            }

            const auto& enumedArray = def->enumedArrays[property.type.u.enumedArrayIndex];
            if (enumedArray.enumIndex < 0 || enumedArray.enumIndex >= def->enumCount || def->enums == nullptr)
            {
                assert(false);
                return 0;
            }

            const auto elementSize = enumedArray.elementType.type == DATA_BOOL ? 1 : enumedArray.elementSize * 8;
            return elementSize * static_cast<size_t>(def->enums[enumedArray.enumIndex].reservedEntryCount);
        }
    default:
        assert(false);
        return 0;
    }
}

size_t AssetDumperStructuredDataDefSet::GetPropertyAlign(const StructuredDataStructProperty& property)
{
    return property.type.type == DATA_BOOL ? 0 : 8;
}

void AssetDumperStructuredDataDefSet::DumpProperty(StructuredDataDefDumper& dumper, const StructuredDataStructProperty& property, const StructuredDataDef* def, const int rootStructIndex)
{
    dumper.BeginProperty(property.name, property.type.type == DATA_BOOL ? property.offset : property.offset * 8, GetPropertySizeInBits(property, def), GetPropertyAlign(property));

    auto currentType = property.type;
    auto stopTypeIteration = false;
    do
    {
        switch (currentType.type)
        {
        case DATA_INT:
            dumper.SetPropertyTypeName("int");
            stopTypeIteration = true;
            break;
        case DATA_BYTE:
            dumper.SetPropertyTypeName("byte");
            stopTypeIteration = true;
            break;
        case DATA_BOOL:
            dumper.SetPropertyTypeName("bool");
            stopTypeIteration = true;
            break;
        case DATA_FLOAT:
            dumper.SetPropertyTypeName("float");
            stopTypeIteration = true;
            break;
        case DATA_SHORT:
            dumper.SetPropertyTypeName("short");
            stopTypeIteration = true;
            break;
        case DATA_STRING:
            {
                std::ostringstream ss;
                ss << "string(" << currentType.u.stringDataLength << ")";
                dumper.SetPropertyTypeName(ss.str());
                stopTypeIteration = true;
                break;
            }
        case DATA_ENUM:
            {
                std::ostringstream ss;
                ss << "ENUM_" << currentType.u.enumIndex;
                dumper.SetPropertyTypeName(ss.str());
                stopTypeIteration = true;
                break;
            }
        case DATA_STRUCT:
            {
                if (currentType.u.structIndex == rootStructIndex)
                {
                    dumper.SetPropertyTypeName("root");
                }
                else
                {
                    std::ostringstream ss;
                    ss << "STRUCT_" << currentType.u.enumIndex;
                    dumper.SetPropertyTypeName(ss.str());
                }
                stopTypeIteration = true;
                break;
            }
        case DATA_INDEXED_ARRAY:
            {
                if (def->indexedArrays == nullptr
                    || currentType.u.indexedArrayIndex < 0
                    || currentType.u.indexedArrayIndex >= def->indexedArrayCount)
                {
                    assert(false);
                    dumper.SetPropertyTypeName("ERRORTYPE");
                    stopTypeIteration = true;
                }
                else
                {
                    const auto& indexedArray = def->indexedArrays[currentType.u.indexedArrayIndex];
                    dumper.AddPropertyArraySpecifier(std::to_string(indexedArray.arraySize));
                    currentType = indexedArray.elementType;
                }
                break;
            }
        case DATA_ENUM_ARRAY:
            {
                if (def->enumedArrays == nullptr
                    || currentType.u.enumedArrayIndex < 0
                    || currentType.u.enumedArrayIndex >= def->enumedArrayCount)
                {
                    assert(false);
                    dumper.SetPropertyTypeName("ERRORTYPE");
                    stopTypeIteration = true;
                }
                else
                {
                    const auto& enumedArray = def->enumedArrays[currentType.u.enumedArrayIndex];
                    std::ostringstream ss;
                    ss << "ENUM_" << enumedArray.enumIndex;
                    dumper.AddPropertyArraySpecifier(ss.str());
                    currentType = enumedArray.elementType;
                }
                break;
            }

        default:
            break;
        }
    }
    while (!stopTypeIteration);

    dumper.EndProperty();
}

void AssetDumperStructuredDataDefSet::DumpStruct(StructuredDataDefDumper& dumper, const size_t structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def,
                                                 const int rootStructIndex)
{
    std::string structName;
    size_t actualStructSize;
    size_t initialOffset;
    if (static_cast<int>(structIndex) == rootStructIndex)
    {
        structName = "root";
        actualStructSize = def->size;
        initialOffset = 64;
    }
    else
    {
        std::ostringstream ss;
        ss << "STRUCT_" << structIndex;
        structName = ss.str();
        actualStructSize = static_cast<size_t>(_struct->size);
        initialOffset = 0;
    }

    dumper.WriteLineComment("BitOffset: "s + std::to_string(_struct->bitOffset));
    dumper.WriteLineComment("Size: "s + std::to_string(_struct->size));
    dumper.BeginStruct(structName, static_cast<size_t>(_struct->propertyCount), actualStructSize * 8, initialOffset);

    if (_struct->properties && _struct->propertyCount > 0)
    {
        for (auto i = 0; i < _struct->propertyCount; i++)
        {
            const auto& property = _struct->properties[i];

            DumpProperty(dumper, property, def, rootStructIndex);
        }
    }

    dumper.EndStruct();
}

CommonStructuredDataType AssetDumperStructuredDataDefSet::ConvertType(const CommonStructuredDataDef* def, const StructuredDataType in)
{
    CommonStructuredDataType out;

    switch (in.type)
    {
    case DATA_INT:
        out.m_category = CommonStructuredDataTypeCategory::INT;
        break;
    case DATA_BYTE:
        out.m_category = CommonStructuredDataTypeCategory::BYTE;
        break;
    case DATA_BOOL:
        out.m_category = CommonStructuredDataTypeCategory::BOOL;
        break;
    case DATA_FLOAT:
        out.m_category = CommonStructuredDataTypeCategory::FLOAT;
        break;
    case DATA_SHORT:
        out.m_category = CommonStructuredDataTypeCategory::SHORT;
        break;
    case DATA_STRING:
        out.m_category = CommonStructuredDataTypeCategory::STRING;
        out.m_info.string_length = in.u.stringDataLength;
        break;
    case DATA_ENUM:
        assert(!def->m_enums.empty());
        out.m_category = CommonStructuredDataTypeCategory::ENUM;
        out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.enumIndex), def->m_enums.size() - 1u), 0u);
        break;
    case DATA_STRUCT:
        assert(!def->m_structs.empty());
        out.m_category = CommonStructuredDataTypeCategory::STRUCT;
        out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.structIndex), def->m_structs.size() - 1u), 0u);
        break;
    case DATA_INDEXED_ARRAY:
        assert(!def->m_indexed_arrays.empty());
        out.m_category = CommonStructuredDataTypeCategory::INDEXED_ARRAY;
        out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.indexedArrayIndex), def->m_indexed_arrays.size() - 1u), 0u);
        break;
    case DATA_ENUM_ARRAY:
        assert(!def->m_enumed_arrays.empty());
        out.m_category = CommonStructuredDataTypeCategory::ENUM_ARRAY;
        out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.enumedArrayIndex), def->m_enumed_arrays.size() - 1u), 0u);
        break;
    case DATA_COUNT:
    default:
        assert(false);
        break;
    }

    return out;
}

void AssetDumperStructuredDataDefSet::ConvertEnum(CommonStructuredDataEnum* out, const StructuredDataEnum* in, const size_t enumIndex)
{
    out->m_name = "ENUM_" + std::to_string(enumIndex);
    out->m_reserved_entry_count = std::max(in->reservedEntryCount, 0);

    out->m_entries.resize(static_cast<size_t>(std::max(in->entryCount, 0)));
    for(auto i = 0u; i < out->m_entries.size(); i++)
    {
        auto& outEntry = out->m_entries[i];
        const auto& inEntry = in->entries[i];

        outEntry.m_name = std::string(inEntry.string);
        outEntry.m_value = inEntry.index;
    }

    out->SortEntries();
}

void AssetDumperStructuredDataDefSet::ConvertStruct(const CommonStructuredDataDef* def, CommonStructuredDataStruct* out, const StructuredDataStruct* in, const size_t structIndex, const size_t rootIndex)
{
    if (structIndex == rootIndex)
        out->m_name = "root";
    else
        out->m_name = "STRUCT_" + std::to_string(structIndex);

    out->m_size_in_byte = static_cast<size_t>(std::max(in->size, 0));
    out->m_bit_offset = in->bitOffset;

    out->m_properties.resize(static_cast<size_t>(std::max(in->propertyCount, 0)));
    for (auto i = 0u; i < out->m_properties.size(); i++)
    {
        auto& outProperty = out->m_properties[i];
        const auto& inProperty = in->properties[i];

        outProperty.m_name = std::string(inProperty.name);
        outProperty.m_type = ConvertType(def, inProperty.type);

        if (inProperty.type.type == DATA_BOOL)
            outProperty.m_offset_in_bits = inProperty.offset;
        else
            outProperty.m_offset_in_bits = inProperty.offset * 8;
    }

    out->SortProperties();
}

void AssetDumperStructuredDataDefSet::ConvertIndexedArray(const CommonStructuredDataDef* def, CommonStructuredDataIndexedArray* out, const StructuredDataIndexedArray* in)
{
    out->m_element_count = static_cast<size_t>(std::max(in->arraySize, 0));
    out->m_element_size_in_bits = in->elementType.type == DATA_BOOL ? 1 : in->elementSize * 8;
    out->m_array_type = ConvertType(def, in->elementType);
}

void AssetDumperStructuredDataDefSet::ConvertEnumedArray(const CommonStructuredDataDef* def, CommonStructuredDataEnumedArray* out, const StructuredDataEnumedArray* in)
{
    assert(!def->m_enums.empty());
    out->m_element_size_in_bits = in->elementType.type == DATA_BOOL ? 1 : in->elementSize * 8;
    out->m_array_type = ConvertType(def, in->elementType);
    out->m_enum_index = std::max(std::min(static_cast<size_t>(in->enumIndex), def->m_enums.size() - 1u), 0u);

    if (def->m_enums.empty())
        return;

    out->m_element_count = def->m_enums[out->m_enum_index]->ElementCount();
}

std::unique_ptr<CommonStructuredDataDef> AssetDumperStructuredDataDefSet::ConvertDef(const StructuredDataDef* in)
{
    auto out = std::make_unique<CommonStructuredDataDef>();

    out->m_version = in->version;
    out->m_checksum = in->formatChecksum;
    out->m_size_in_byte = in->size;

    out->m_enums.resize(static_cast<size_t>(std::max(in->enumCount, 0)));
    out->m_structs.resize(static_cast<size_t>(std::max(in->structCount, 0)));
    out->m_indexed_arrays.resize(static_cast<size_t>(std::max(in->indexedArrayCount, 0)));
    out->m_enumed_arrays.resize(static_cast<size_t>(std::max(in->enumedArrayCount, 0)));

    for (auto i = 0u; i < out->m_enums.size(); i++)
    {
        auto _enum = std::make_unique<CommonStructuredDataEnum>();
        ConvertEnum(_enum.get(), &in->enums[i], i);
        out->m_enums[i] = std::move(_enum);
    }
    for (auto i = 0u; i < out->m_structs.size(); i++)
    {
        auto _struct = std::make_unique<CommonStructuredDataStruct>();
        ConvertStruct(out.get(), _struct.get(), &in->structs[i], i, static_cast<size_t>(in->rootType.u.structIndex));
        out->m_structs[i] = std::move(_struct);
    }
    for (auto i = 0u; i < out->m_indexed_arrays.size(); i++)
        ConvertIndexedArray(out.get(), &out->m_indexed_arrays[i], &in->indexedArrays[i]);
    for (auto i = 0u; i < out->m_enumed_arrays.size(); i++)
        ConvertEnumedArray(out.get(), &out->m_enumed_arrays[i], &in->enumedArrays[i]);

    out->m_root_type = ConvertType(out.get(), in->rootType);

    return out;
}

void AssetDumperStructuredDataDefSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset)
{
    const auto* set = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile || set->defs == nullptr)
        return;

    StructuredDataDefDumperNew newDumper(*assetFile);
    for (auto i = 0u; i < set->defCount; i++)
    {
        const auto def = ConvertDef(&set->defs[i]);
        newDumper.DumpDef(*def);
    }
}
