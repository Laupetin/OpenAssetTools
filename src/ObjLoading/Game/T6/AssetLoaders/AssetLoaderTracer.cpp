#include "AssetLoaderTracer.h"

#include <cstring>
#include <iostream>
#include <cassert>

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/TracerFields.h"
#include "InfoString/InfoString.h"

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
                return ConvertEnumInt(value, field.iOffset, tracerTypeNames, std::extent<decltype(tracerTypeNames)>::value);

            case TFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToTracerConverter(const InfoString& infoString, TracerDef* tracer, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager,
            const cspField_t* fields, const size_t fieldCount)
            : InfoStringToStructConverter(infoString, tracer, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
}

void* AssetLoaderTracer::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* tracer = memory->Create<TracerDef>();
    memset(tracer, 0, sizeof(TracerDef));
    tracer->name = memory->Dup(assetName.c_str());
    return tracer;
}

bool AssetLoaderTracer::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderTracer::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "tracer/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_TRACER, *file.m_stream))
    {
        std::cout << "Failed to read tracer raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    auto* tracer = memory->Create<TracerDef>();
    memset(tracer, 0, sizeof(TracerDef));

    InfoStringToTracerConverter converter(infoString, tracer, zone->m_script_strings, memory, manager, tracer_fields, std::extent<decltype(tracer_fields)>::value);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse tracer raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }
    
    tracer->name = memory->Dup(assetName.c_str());

    manager->AddAsset(ASSET_TYPE_TRACER, assetName, tracer, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}
