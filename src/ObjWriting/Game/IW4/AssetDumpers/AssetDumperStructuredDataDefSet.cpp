#include "AssetDumperStructuredDataDefSet.h"

#include <cassert>
#include <sstream>

#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"

using namespace IW4;

bool AssetDumperStructuredDataDefSet::GetNextHeadValue(const StructuredDataDef* def, const bool isFirstStruct, const std::vector<bool>& structsIncludedInOrder, size_t& nextHeadValue)
{
    if(isFirstStruct
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
    while(true)
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

    while(resultStructTail < structCount)
    {
        size_t nextHeadValue;
        if (!GetNextHeadValue(def, resultStructHead == 0, structIncludedInOrder, nextHeadValue))
            return result;
        result.push_back(nextHeadValue);
        structIncludedInOrder[nextHeadValue] = true;
        ++resultStructHead;

        while(resultStructHead > resultStructTail)
        {
            const auto& currentStruct = def->structs[result[resultStructTail++]];

            if(currentStruct.properties == nullptr)
                continue;

            for(auto i = 0; i < currentStruct.propertyCount; i++)
            {
                const auto baseType = GetBaseType(def, currentStruct.properties[i].type);
                if(baseType.type == DATA_STRUCT 
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
    if (!_enum->entries || _enum->entryCount <= 0)
        return;

    std::ostringstream ss;
    ss << "ENUM_" << enumIndex;

    dumper.BeginEnum(ss.str(), static_cast<size_t>(_enum->entryCount));

    for (auto i = 0; i < _enum->entryCount; i++)
    {
        const auto& entry = _enum->entries[i];

        assert(entry.string);
        if (!entry.string)
            continue;

        dumper.WriteEnumEntry(entry.string, entry.index);
    }

    dumper.EndEnum();
}

void AssetDumperStructuredDataDefSet::DumpProperty(StructuredDataDefDumper& dumper, const StructuredDataStructProperty& property, const StructuredDataDef* def, const int rootStructIndex)
{
    dumper.BeginProperty(property.name, property.offset);

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

void AssetDumperStructuredDataDefSet::DumpStruct(StructuredDataDefDumper& dumper, const size_t structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def, const int rootStructIndex)
{
    if (!_struct->properties || _struct->propertyCount <= 0)
        return;

    std::string structName;
    if (static_cast<int>(structIndex) == rootStructIndex)
    {
        structName = "root";
    }
    else
    {
        std::ostringstream ss;
        ss << "STRUCT_" << structIndex;
        structName = ss.str();
    }

    dumper.BeginStruct(structName, static_cast<size_t>(_struct->propertyCount));
    
    for (auto i = 0; i < _struct->propertyCount; i++)
    {
        const auto& property = _struct->properties[i];

        DumpProperty(dumper, property, def, rootStructIndex);
    }

    dumper.EndStruct();
}

void AssetDumperStructuredDataDefSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset)
{
    const auto* set = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile || set->defs == nullptr)
        return;

    StructuredDataDefDumper dumper(*assetFile);

    for (auto defIndex = 0u; defIndex < set->defCount; defIndex++)
    {
        const auto& def = set->defs[defIndex];

        dumper.BeginVersion(def.version);

        for (auto enumIndex = 0; enumIndex < def.enumCount; enumIndex++)
            DumpEnum(dumper, enumIndex, &def.enums[enumIndex]);

        const auto rootStructIndex = def.rootType.type == DATA_STRUCT ? def.rootType.u.structIndex : -1;
        assert(rootStructIndex >= 0);
        const auto structDumpingOrder = CalculateStructDumpingOrder(&def);
        for (auto i = structDumpingOrder.rbegin(); i != structDumpingOrder.rend(); ++i)
            DumpStruct(dumper, *i, &def.structs[*i], &def, rootStructIndex);

        dumper.EndVersion();
    }
}
