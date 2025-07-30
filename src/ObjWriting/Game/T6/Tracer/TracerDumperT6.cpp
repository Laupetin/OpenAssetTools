#include "TracerDumperT6.h"

#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/Tracer/TracerFields.h"
#include "Tracer/TracerCommon.h"

#include <cassert>
#include <type_traits>

using namespace T6;
using namespace ::tracer;

namespace
{
    class InfoStringFromTracerConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<tracerFieldType_t>(field.iFieldType))
            {
            case TFT_TRACERTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, tracerTypeNames, std::extent_v<decltype(tracerTypeNames)>);
                break;

            case TFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromTracerConverter(const TracerDef* structure,
                                      const cspField_t* fields,
                                      const size_t fieldCount,
                                      std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };

    InfoString CreateInfoString(XAssetInfo<TracerDef>* asset)
    {
        InfoStringFromTracerConverter converter(asset->Asset(),
                                                tracer_fields,
                                                std::extent_v<decltype(tracer_fields)>,
                                                [asset](const scr_string_t scrStr) -> std::string
                                                {
                                                    assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                    if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                        return "";

                                                    return asset->m_zone->m_script_strings[scrStr];
                                                });

        return converter.Convert();
    }
} // namespace

namespace T6::tracer
{
    bool Dumper::ShouldDump(XAssetInfo<TracerDef>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset)
    {
        // Only dump raw when no gdt available
        if (context.m_gdt)
        {
            const auto infoString = CreateInfoString(asset);
            GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_TRACER);
            infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_TRACER, gdtEntry);
            context.m_gdt->WriteEntry(gdtEntry);
        }
        else
        {
            const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset->m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(asset);
            const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_TRACER);
            stream.write(stringValue.c_str(), stringValue.size());
        }
    }
} // namespace T6::tracer
