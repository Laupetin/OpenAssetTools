#include "ZoneWriting.h"

#include "Game/IW4/ZoneWriterFactoryIW4.h"
#include "Game/T6/ZoneWriterFactoryT6.h"
#include "Writing/IZoneWriterFactory.h"

IZoneWriterFactory* ZoneWriterFactories[]
{
    new IW4::ZoneWriterFactory(),
    new T6::ZoneWriterFactory()
};

bool ZoneWriting::WriteZone(std::ostream& stream, Zone* zone)
{
    return true;
}
