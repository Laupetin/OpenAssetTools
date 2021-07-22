#pragma once
#include "AssetLoading/IAssetLoadingManager.h"
#include "InfoString/InfoStringToStructConverterBase.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class InfoStringToStructConverter : public InfoStringToStructConverterBase
    {
    protected:
        IAssetLoadingManager* m_loading_manager;
        const cspField_t* m_fields;
        size_t m_field_count;

        static bool GetHashValue(const std::string& value, unsigned int& hash);

        virtual bool ConvertExtensionField(const cspField_t& field, const std::string& value) = 0;
        bool ConvertBaseField(const cspField_t& field, const std::string& value);

    public:
        InfoStringToStructConverter(const InfoString& infoString, void* structure, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager, const cspField_t* fields,
            size_t fieldCount);
        bool Convert() override;
    };
}