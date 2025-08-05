#include "InfoStringLoaderTracerT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/Tracer/TracerFields.h"
#include "Tracer/TracerCommon.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
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
                                    TracerDef& tracer,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    AssetRegistration<AssetTracer>& registration,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &tracer, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };
} // namespace

namespace tracer
{
    InfoStringLoaderT6::InfoStringLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderT6::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* tracer = m_memory.Alloc<TracerDef>();
        tracer->name = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetTracer> registration(assetName, tracer);

        InfoStringToTracerConverter converter(
            infoString, *tracer, m_zone.m_script_strings, m_memory, context, registration, tracer_fields, std::extent_v<decltype(tracer_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse tracer: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }

} // namespace tracer
