#pragma once

#include "InfoString/InfoString.h"

class InfoStringToStructConverterBase
{
protected:
    const InfoString& m_info_string;
    void* m_structure;

    virtual void FillStructure() = 0;

public:
    InfoStringToStructConverterBase(const InfoString& infoString, void* structure);
    virtual ~InfoStringToStructConverterBase();
    InfoStringToStructConverterBase(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase(InfoStringToStructConverterBase&& other) noexcept = delete;
    InfoStringToStructConverterBase& operator=(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase& operator=(InfoStringToStructConverterBase&& other) noexcept = delete;

    void Convert();
};