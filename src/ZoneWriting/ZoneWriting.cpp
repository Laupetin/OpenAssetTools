#include "ZoneWriting.h"

#include "Utils/Logging/Log.h"
#include "Writing/IZoneWriterFactory.h"

#include <chrono>
#include <format>
#include <iostream>

bool ZoneWriting::WriteZone(std::ostream& stream, const Zone& zone)
{
    const auto start = std::chrono::high_resolution_clock::now();

    const auto factory = IZoneWriterFactory::GetZoneWriterFactoryForGame(zone.m_game_id);

    const auto zoneWriter = factory->CreateWriter(zone);
    if (zoneWriter == nullptr)
    {
        con::error("Could not create ZoneWriter for zone \"{}\".", zone.m_name);
        return false;
    }

    const auto result = zoneWriter->WriteZone(stream);

    const auto end = std::chrono::high_resolution_clock::now();

    con::info("Writing zone \"{}\" took {} ms.", zone.m_name, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return result;
}
