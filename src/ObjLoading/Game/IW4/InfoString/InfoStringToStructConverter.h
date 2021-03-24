#pragma once
#include "InfoString/InfoStringToStructConverterBase.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class InfoStringToStructConverter : public InfoStringToStructConverterBase
    {
        const cspField_t* m_fields;
        size_t m_field_count;

    protected:
        void FillStructure() override;

    public:
        InfoStringToStructConverter(const InfoString& infoString, void* structure, const cspField_t* fields, size_t fieldCount);
    };
}