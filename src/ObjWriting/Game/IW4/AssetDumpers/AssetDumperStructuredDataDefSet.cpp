#include "AssetDumperStructuredDataDefSet.h"

#include <cassert>
#include <sstream>

#include "Dumping/StructuredDataDef/StructuredDataDefDumper.h"

using namespace IW4;

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
    dumper.BeginProperty(property.name);

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

void AssetDumperStructuredDataDefSet::DumpStruct(StructuredDataDefDumper& dumper, const int structIndex, const StructuredDataStruct* _struct, const StructuredDataDef* def, const bool isRoot)
{
    if (!_struct->properties || _struct->propertyCount <= 0)
        return;

    std::string structName;

    if (isRoot)
    {
        structName = "root";
    }
    else
    {
        std::ostringstream ss;
        ss << "STRUCT_" << structIndex;
        structName = ss.str();
    }

    dumper.BeginStruct(structName);

    const auto rootStructIndex = def->rootType.type == DATA_STRUCT ? def->rootType.u.structIndex : -1;
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
        for (auto structIndex = 0; structIndex < def.structCount; structIndex++)
            DumpStruct(dumper, structIndex, &def.structs[structIndex], &def, structIndex == rootStructIndex);

        dumper.EndVersion();
    }
}
