#include "AssetDumperTracer.h"

#include <cassert>

#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperTracer::tracer_fields[]
{
    { "type", offsetof(TracerDef, type), TFT_TRACERTYPE },
    { "material", offsetof(TracerDef, material), CSPFT_MATERIAL },
    { "drawInterval", offsetof(TracerDef, drawInterval), CSPFT_INT },
    { "speed", offsetof(TracerDef, speed), CSPFT_FLOAT },
    { "beamLength", offsetof(TracerDef, beamLength), CSPFT_FLOAT },
    { "beamWidth", offsetof(TracerDef, beamWidth), CSPFT_FLOAT },
    { "screwRadius", offsetof(TracerDef, screwRadius), CSPFT_FLOAT },
    { "screwDist", offsetof(TracerDef, screwDist), CSPFT_FLOAT },
    { "fadeTime", offsetof(TracerDef, fadeTime), CSPFT_FLOAT },
    { "fadeScale", offsetof(TracerDef, fadeScale), CSPFT_FLOAT },
    { "texRepeatRate", offsetof(TracerDef, texRepeatRate), CSPFT_FLOAT },
    { "colorR0", offsetof(TracerDef, colors[0].r), CSPFT_FLOAT },
    { "colorG0", offsetof(TracerDef, colors[0].g), CSPFT_FLOAT },
    { "colorB0", offsetof(TracerDef, colors[0].b), CSPFT_FLOAT },
    { "colorA0", offsetof(TracerDef, colors[0].a), CSPFT_FLOAT },
    { "colorR1", offsetof(TracerDef, colors[1].r), CSPFT_FLOAT },
    { "colorG1", offsetof(TracerDef, colors[1].g), CSPFT_FLOAT },
    { "colorB1", offsetof(TracerDef, colors[1].b), CSPFT_FLOAT },
    { "colorA1", offsetof(TracerDef, colors[1].a), CSPFT_FLOAT },
    { "colorR2", offsetof(TracerDef, colors[2].r), CSPFT_FLOAT },
    { "colorG2", offsetof(TracerDef, colors[2].g), CSPFT_FLOAT },
    { "colorB2", offsetof(TracerDef, colors[2].b), CSPFT_FLOAT },
    { "colorA2", offsetof(TracerDef, colors[2].a), CSPFT_FLOAT },
    { "colorR3", offsetof(TracerDef, colors[3].r), CSPFT_FLOAT },
    { "colorG3", offsetof(TracerDef, colors[3].g), CSPFT_FLOAT },
    { "colorB3", offsetof(TracerDef, colors[3].b), CSPFT_FLOAT },
    { "colorA3", offsetof(TracerDef, colors[3].a), CSPFT_FLOAT },
    { "colorR4", offsetof(TracerDef, colors[4].r), CSPFT_FLOAT },
    { "colorG4", offsetof(TracerDef, colors[4].g), CSPFT_FLOAT },
    { "colorB4", offsetof(TracerDef, colors[4].b), CSPFT_FLOAT },
    { "colorA4", offsetof(TracerDef, colors[4].a), CSPFT_FLOAT }
};

namespace T6
{
    const char* tracerTypeNames[]
    {
        "Laser",
        "Smoke"
    };

    class InfoStringFromTracerConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<tracerFieldType_t>(field.iFieldType))
            {
            case TFT_TRACERTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, tracerTypeNames, _countof(tracerTypeNames));
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

bool AssetDumperTracer::ShouldDump(XAssetInfo<TracerDef>* asset)
{
    return true;
}

std::string AssetDumperTracer::GetFileNameForAsset(Zone* zone, XAssetInfo<TracerDef>* asset)
{
    return "tracer/" + asset->m_name;
}

void AssetDumperTracer::DumpAsset(Zone* zone, XAssetInfo<TracerDef>* asset, FileAPI::File* out)
{
    InfoStringFromTracerConverter converter(asset->Asset(), tracer_fields, _countof(tracer_fields), [asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset->m_zone->m_script_strings.size());
            if (scrStr >= asset->m_zone->m_script_strings.size())
                return "";

            return asset->m_zone->m_script_strings[scrStr];
        });

    const auto infoString = converter.Convert();
    const auto stringValue = infoString.ToString("TRACER");
    out->Write(stringValue.c_str(), 1, stringValue.length());
}

//void AssetDumperTracer::CheckFields()
//{
//    assert(_countof(tracer_fields) == _countof(fields222));
//
//    for(auto i = 0u; i < _countof(tracer_fields); i++)
//    {
//        if(tracer_fields[i].iOffset != fields222[i].iOffset)
//        {
//            std::string error = "Error in field: " + std::string(tracer_fields[i].szName);
//            MessageBoxA(NULL, error.c_str(), "", 0);
//            exit(0);
//        }
//    }
//
//    MessageBoxA(NULL, "No error", "", 0);
//    exit(0);
//}