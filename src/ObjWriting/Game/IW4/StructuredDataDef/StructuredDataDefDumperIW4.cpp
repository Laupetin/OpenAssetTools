#include "StructuredDataDefDumperIW4.h"

#include "StructuredDataDef/StructuredDataDefDumper.h"

#include <algorithm>
#include <cassert>
#include <sstream>

using namespace IW4;
using namespace std::string_literals;

namespace
{
    CommonStructuredDataType ConvertType(const CommonStructuredDataDef* def, const StructuredDataType in)
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
            out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.enumIndex), def->m_enums.size() - 1uz), 0uz);
            break;
        case DATA_STRUCT:
            assert(!def->m_structs.empty());
            out.m_category = CommonStructuredDataTypeCategory::STRUCT;
            out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.structIndex), def->m_structs.size() - 1uz), 0uz);
            break;
        case DATA_INDEXED_ARRAY:
            assert(!def->m_indexed_arrays.empty());
            out.m_category = CommonStructuredDataTypeCategory::INDEXED_ARRAY;
            out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.indexedArrayIndex), def->m_indexed_arrays.size() - 1uz), 0uz);
            break;
        case DATA_ENUM_ARRAY:
            assert(!def->m_enumed_arrays.empty());
            out.m_category = CommonStructuredDataTypeCategory::ENUM_ARRAY;
            out.m_info.type_index = std::max(std::min(static_cast<size_t>(in.u.enumedArrayIndex), def->m_enumed_arrays.size() - 1uz), 0uz);
            break;
        case DATA_COUNT:
        default:
            assert(false);
            break;
        }

        return out;
    }

    void ConvertEnum(CommonStructuredDataEnum* out, const StructuredDataEnum* in, const size_t enumIndex)
    {
        out->m_name = "ENUM_" + std::to_string(enumIndex);

        if (in->reservedEntryCount > 0 && in->reservedEntryCount != in->entryCount)
            out->m_reserved_entry_count = std::max(in->reservedEntryCount, 0);
        else
            out->m_reserved_entry_count = -1;

        out->m_entries.resize(static_cast<size_t>(std::max(in->entryCount, 0)));
        for (auto i = 0u; i < out->m_entries.size(); i++)
        {
            auto& outEntry = out->m_entries[i];
            const auto& inEntry = in->entries[i];

            outEntry.m_name = std::string(inEntry.string);
            outEntry.m_value = inEntry.index;
        }

        out->SortEntriesByOffset();
    }

    void ConvertStruct(const CommonStructuredDataDef* def,
                       const StructuredDataDef* gameDef,
                       CommonStructuredDataStruct* out,
                       const StructuredDataStruct* in,
                       const size_t structIndex)
    {
        if (gameDef->rootType.type == DATA_STRUCT && structIndex == static_cast<size_t>(gameDef->rootType.u.structIndex))
        {
            out->m_name = "root";
            out->m_size_in_byte = gameDef->size;
        }
        else
        {
            out->m_name = "STRUCT_" + std::to_string(structIndex);
            out->m_size_in_byte = static_cast<size_t>(std::max(in->size, 0));
        }

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

        out->SortPropertiesByOffset();
    }

    void ConvertIndexedArray(const CommonStructuredDataDef* def, CommonStructuredDataIndexedArray* out, const StructuredDataIndexedArray* in)
    {
        out->m_element_count = static_cast<size_t>(std::max(in->arraySize, 0));
        out->m_element_size_in_bits = in->elementType.type == DATA_BOOL ? 1 : in->elementSize * 8;
        out->m_array_type = ConvertType(def, in->elementType);
    }

    void ConvertEnumedArray(const CommonStructuredDataDef* def, CommonStructuredDataEnumedArray* out, const StructuredDataEnumedArray* in)
    {
        assert(!def->m_enums.empty());
        out->m_element_size_in_bits = in->elementType.type == DATA_BOOL ? 1 : in->elementSize * 8;
        out->m_array_type = ConvertType(def, in->elementType);
        out->m_enum_index = std::max(std::min(static_cast<size_t>(in->enumIndex), def->m_enums.size() - 1uz), 0uz);

        if (def->m_enums.empty())
        {
            assert(false);
            return;
        }

        out->m_element_count = def->m_enums[out->m_enum_index]->ElementCount();
    }

    std::unique_ptr<CommonStructuredDataDef> ConvertDef(const StructuredDataDef* in)
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
            ConvertStruct(out.get(), in, _struct.get(), &in->structs[i], i);
            out->m_structs[i] = std::move(_struct);
        }
        for (auto i = 0u; i < out->m_indexed_arrays.size(); i++)
            ConvertIndexedArray(out.get(), &out->m_indexed_arrays[i], &in->indexedArrays[i]);
        for (auto i = 0u; i < out->m_enumed_arrays.size(); i++)
            ConvertEnumedArray(out.get(), &out->m_enumed_arrays[i], &in->enumedArrays[i]);

        out->m_root_type = ConvertType(out.get(), in->rootType);

        return out;
    }
} // namespace

namespace structured_data_def
{
    bool DumperIW4::ShouldDump(XAssetInfo<StructuredDataDefSet>* asset)
    {
        return true;
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset)
    {
        const auto* set = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile || set->defs == nullptr)
            return;

        StructuredDataDefDumper dumper(*assetFile);
        for (auto i = 0u; i < set->defCount; i++)
        {
            const auto def = ConvertDef(&set->defs[i]);
            dumper.DumpDef(*def);
        }
    }
} // namespace structured_data_def
