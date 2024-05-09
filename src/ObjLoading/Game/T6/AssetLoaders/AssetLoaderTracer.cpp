#include "AssetLoaderTracer.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/TracerFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <cassert>
#include <cstring>
#include <iostream>

using namespace T6;

namespace T6
{
    class InfoStringToTracerConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<tracerFieldType_t>(field.iFieldType))
            {
            case TFT_TRACERTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, tracerTypeNames, std::extent_v<decltype(tracerTypeNames)>);

            case TFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToTracerConverter(const InfoString& infoString,
                                    TracerDef* tracer,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager* memory,
                                    IAssetLoadingManager* manager,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, tracer, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

bool AssetLoaderTracer::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    auto* tracer = memory->Create<TracerDef>();
    memset(tracer, 0, sizeof(TracerDef));

    InfoStringToTracerConverter converter(infoString, tracer, zone->m_script_strings, memory, manager, tracer_fields, std::extent_v<decltype(tracer_fields)>);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse tracer: \"" << assetName << "\"\n";
        return true;
    }

    tracer->name = memory->Dup(assetName.c_str());

    manager->AddAsset<AssetTracer>(assetName, tracer, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}

void* AssetLoaderTracer::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* tracer = memory->Create<TracerDef>();
    memset(tracer, 0, sizeof(TracerDef));
    tracer->name = memory->Dup(assetName.c_str());
    return tracer;
}

bool AssetLoaderTracer::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderTracer::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_TRACER, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read tracer gdt entry: \"" << assetName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderTracer::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderTracer::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "tracer/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_TRACER, *file.m_stream))
    {
        std::cout << "Failed to read tracer raw file: \"" << fileName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
