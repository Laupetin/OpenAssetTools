#pragma once

#include <string>

#include "Obj/Gdt/GdtStream.h"
#include "Zone/Zone.h"

class AssetDumpingContext
{
public:
    Zone* m_zone;
    std::string m_base_path;
    std::unique_ptr<GdtOutputStream> m_gdt;

    AssetDumpingContext();
};
