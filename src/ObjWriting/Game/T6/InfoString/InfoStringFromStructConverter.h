#pragma once
#include "Game/T6/T6.h"
#include "InfoString/InfoStringFromStructConverterBase.h"

namespace T6
{
    class InfoStringFromStructConverter : public InfoStringFromStructConverterBase
    {
    protected:
        const cspField_t* m_fields;
        size_t m_field_count;

        virtual void FillFromExtensionField(const cspField_t& field) = 0;
        void FillFromBaseField(const cspField_t& field);
        void FillInfoString() override;

    public:
        InfoStringFromStructConverter(const void* structure, const cspField_t* fields, size_t fieldCount);
        InfoStringFromStructConverter(const void* structure,
                                      const cspField_t* fields,
                                      size_t fieldCount,
                                      std::function<std::string(scr_string_t)> scriptStringValueCallback);
    };
} // namespace T6
