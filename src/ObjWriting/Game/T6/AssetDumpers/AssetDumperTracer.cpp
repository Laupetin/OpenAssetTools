#include "AssetDumperTracer.h"

#include <cassert>
#include <type_traits>

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/TracerFields.h"

using namespace T6;

namespace T6
{
    class InfoStringFromTracerConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<tracerFieldType_t>(field.iFieldType))
            {
            case TFT_TRACERTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, tracerTypeNames, std::extent<decltype(tracerTypeNames)>::value);
                break;

            case TFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromTracerConverter(const TracerDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperTracer::CreateInfoString(XAssetInfo<TracerDef>* asset)
{
    InfoStringFromTracerConverter converter(asset->Asset(), tracer_fields, std::extent<decltype(tracer_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
    {
        assert(scrStr < asset->m_zone->m_script_strings.Count());
        if (scrStr >= asset->m_zone->m_script_strings.Count())
            return "";

        return asset->m_zone->m_script_strings[scrStr];
    });

    return converter.Convert();
}

bool AssetDumperTracer::ShouldDump(XAssetInfo<TracerDef>* asset)
{
    return true;
}

bool AssetDumperTracer::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperTracer::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperTracer::GetFileNameForAsset(Zone* zone, XAssetInfo<TracerDef>* asset)
{
    return "tracer/" + asset->m_name;
}

GdtEntry AssetDumperTracer::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperTracer::DumpRaw(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString("TRACER");
    stream.write(stringValue.c_str(), stringValue.size());
}
