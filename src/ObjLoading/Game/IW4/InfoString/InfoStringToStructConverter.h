#pragma once
#include "AssetLoading/IAssetLoadingManager.h"
#include "InfoString/InfoStringToStructConverterBase.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class InfoStringToStructConverter : public InfoStringToStructConverterBase
    {
    protected:
        IAssetLoadingManager* m_loading_manager;
        const cspField_t* m_fields;
        size_t m_field_count;

        virtual bool ConvertExtensionField(const cspField_t& field, const std::string& value) = 0;
        bool ConvertBaseField(const cspField_t& field, const std::string& value);

    public:
        InfoStringToStructConverter(const InfoString& infoString, void* structure, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager, const cspField_t* fields,
            size_t fieldCount);
        bool Convert() override;
    };
}