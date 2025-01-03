#pragma once

#include "Obj/Gdt/GdtEntry.h"

#include <string>

class IGdtQueryable
{
public:
    IGdtQueryable() = default;
    virtual ~IGdtQueryable() = default;
    IGdtQueryable(const IGdtQueryable& other) = default;
    IGdtQueryable(IGdtQueryable&& other) noexcept = default;
    IGdtQueryable& operator=(const IGdtQueryable& other) = default;
    IGdtQueryable& operator=(IGdtQueryable&& other) noexcept = default;

    virtual GdtEntry* GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName) = 0;
};
