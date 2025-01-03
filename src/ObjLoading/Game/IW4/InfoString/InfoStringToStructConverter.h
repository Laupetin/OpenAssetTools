#pragma once

#include "Game/IW4/IW4.h"
#include "InfoString/InfoStringToStructConverterBase.h"

namespace IW4
{
    class InfoStringToStructConverter : public InfoStringToStructConverterBase
    {
    public:
        InfoStringToStructConverter(const InfoString& infoString,
                                    void* structure,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    GenericAssetRegistration& registration,
                                    const cspField_t* fields,
                                    size_t fieldCount);
        bool Convert() override;

    protected:
        virtual bool ConvertExtensionField(const cspField_t& field, const std::string& value) = 0;
        bool ConvertBaseField(const cspField_t& field, const std::string& value);

        const cspField_t* m_fields;
        size_t m_field_count;
    };
} // namespace IW4
