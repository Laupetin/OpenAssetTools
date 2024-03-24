#include "AssetDumperZBarrier.h"

#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/ZBarrierFields.h"
#include "Game/T6/ObjConstantsT6.h"

#include <cassert>
#include <type_traits>

using namespace T6;

namespace T6
{
    class InfoStringFromZBarrierConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            assert(false);
        }

    public:
        InfoStringFromZBarrierConverter(const ZBarrierDef* structure,
                                        const cspField_t* fields,
                                        const size_t fieldCount,
                                        std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
} // namespace T6

InfoString AssetDumperZBarrier::CreateInfoString(XAssetInfo<ZBarrierDef>* asset)
{
    InfoStringFromZBarrierConverter converter(asset->Asset(),
                                              zbarrier_fields,
                                              std::extent_v<decltype(zbarrier_fields)>,
                                              [asset](const scr_string_t scrStr) -> std::string
                                              {
                                                  assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                  if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                      return "";

                                                  return asset->m_zone->m_script_strings[scrStr];
                                              });

    return converter.Convert();
}

bool AssetDumperZBarrier::ShouldDump(XAssetInfo<ZBarrierDef>* asset)
{
    return true;
}

void AssetDumperZBarrier::DumpAsset(AssetDumpingContext& context, XAssetInfo<ZBarrierDef>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_ZBARRIER);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_ZBARRIER, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("zbarrier/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_ZBARRIER);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
