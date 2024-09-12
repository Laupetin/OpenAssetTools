#include "ZoneWriting.h"

#include "Game/IW3/ZoneWriterFactoryIW3.h"
#include "Game/IW4/ZoneWriterFactoryIW4.h"
#include "Game/IW5/ZoneWriterFactoryIW5.h"
#include "Game/T5/ZoneWriterFactoryT5.h"
#include "Game/T6/ZoneWriterFactoryT6.h"
#include "Writing/IZoneWriterFactory.h"

#include <chrono>
#include <iostream>

IZoneWriterFactory* ZoneWriterFactories[]{
    new IW3::ZoneWriterFactory(),
    new IW4::ZoneWriterFactory(),
    new IW5::ZoneWriterFactory(),
    new T5::ZoneWriterFactory(),
    new T6::ZoneWriterFactory(),
};

bool ZoneWriting::WriteZone(std::ostream& stream, Zone* zone)
{
    const auto start = std::chrono::high_resolution_clock::now();

    std::unique_ptr<ZoneWriter> zoneWriter;
    for (auto* factory : ZoneWriterFactories)
    {
        if (factory->SupportsZone(zone))
        {
            zoneWriter = factory->CreateWriter(zone);
            break;
        }
    }

    if (zoneWriter == nullptr)
    {
        std::cerr << std::format("Could not create ZoneWriter for zone \"{}\".\n", zone->m_name);
        return false;
    }

    const auto result = zoneWriter->WriteZone(stream);

    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::format("Writing zone \"{}\" took {} msec.\n", zone->m_name, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return result;
}
