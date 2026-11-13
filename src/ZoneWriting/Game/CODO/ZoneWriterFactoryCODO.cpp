#include "ZoneWriterFactoryCODO.h"

using namespace CODO;

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(const Zone& zone) const
{
    // Writing CODO zones is not supported
    return nullptr;
}
