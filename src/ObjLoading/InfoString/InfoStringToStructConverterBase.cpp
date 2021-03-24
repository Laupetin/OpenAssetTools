#include "InfoStringToStructConverterBase.h"

InfoStringToStructConverterBase::InfoStringToStructConverterBase(const InfoString& infoString, void* structure)
    : m_info_string(infoString),
    m_structure(structure)
{
}

InfoStringToStructConverterBase::~InfoStringToStructConverterBase()
= default;

void InfoStringToStructConverterBase::Convert()
{
    FillStructure();
}