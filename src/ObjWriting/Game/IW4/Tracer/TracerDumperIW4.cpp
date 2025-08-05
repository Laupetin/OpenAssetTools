#include "TracerDumperIW4.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/Tracer/TracerFields.h"
#include "Tracer/TracerCommon.h"

#include <cassert>
#include <sstream>
#include <type_traits>

using namespace IW4;

namespace
{
    class InfoStringFromTracerConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            assert(false);
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

namespace tracer
{
    bool DumperIW4::ShouldDump(XAssetInfo<TracerDef>* asset)
    {
        return true;
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset)
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
} // namespace tracer
