#include "InfoStringToStructConverter.h"

#include <cassert>

using namespace T6;

void InfoStringToStructConverter::FillStructure()
{
}

InfoStringToStructConverter::InfoStringToStructConverter(const InfoString& infoString, void* structure,
    const cspField_t* fields, const size_t fieldCount)
    : InfoStringToStructConverterBase(infoString, structure),
    m_fields(fields),
    m_field_count(fieldCount)
{
}