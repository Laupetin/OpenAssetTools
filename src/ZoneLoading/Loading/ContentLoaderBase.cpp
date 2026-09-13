#include "ContentLoaderBase.h"

#include <cassert>
#include <cstdint>
#include <limits>

ContentLoaderBase::ContentLoaderBase(Zone& zone, ZoneInputStream& stream)
    : varXString(nullptr),
      m_zone(zone),
      m_memory(zone.Memory()),
      m_stream(stream),

      // -1
      m_zone_ptr_following(
          reinterpret_cast<const void*>(std::numeric_limits<std::uintptr_t>::max() >> ((sizeof(std::uintptr_t) * 8u) - stream.GetPointerBitCount()))),

      // -2
      m_zone_ptr_insert(
          reinterpret_cast<const void*>((std::numeric_limits<std::uintptr_t>::max() >> ((sizeof(std::uintptr_t) * 8u) - stream.GetPointerBitCount())) - 1u))
{
}

void ContentLoaderBase::LoadXString(const bool atStreamStart) const
{
    assert(varXString != nullptr);

    if (atStreamStart)
        m_stream.Load<const char*>(varXString);

    if (*varXString != nullptr)
    {
        const auto zonePtrType = GetZonePointerType(*varXString);
        if (zonePtrType == ZonePointerType::FOLLOWING || zonePtrType == ZonePointerType::INSERT)
        {
            const char** toInsert = nullptr;
            uintptr_t toInsertLookupEntry = 0;
            if (zonePtrType == ZonePointerType::INSERT)
            {
                if (m_stream.GetPointerBitCount() == sizeof(const char*) * 8u)
                    toInsert = m_stream.InsertPointerNative<const char>();
                else
                    toInsertLookupEntry = m_stream.InsertPointerAliasLookup();
            }

            *varXString = m_stream.Alloc<const char>(1);
            m_stream.LoadNullTerminated(const_cast<char*>(*varXString));

            if (zonePtrType == ZonePointerType::INSERT)
            {
                if (toInsert)
                    *toInsert = *varXString;
                else
                    m_stream.SetInsertedPointerAliasLookup(toInsertLookupEntry, const_cast<char*>(*varXString));
            }
        }
        else
        {
            *varXString = m_stream.ConvertOffsetToPointerNative<const char>(*varXString);
        }
    }
}

void ContentLoaderBase::LoadXStringArray(const bool atStreamStart, const size_t count)
{
    assert(varXString != nullptr);

#ifdef ARCH_x86
    if (atStreamStart)
        m_stream.Load<const char*>(varXString, count);
#else
    if (atStreamStart)
    {
        const auto pointerByteCount = m_stream.GetPointerBitCount() / 8u;
        if (pointerByteCount == sizeof(const char*))
        {
            m_stream.Load<const char*>(varXString, count);
        }
        else
        {
            const auto fill = m_stream.LoadWithFill(pointerByteCount * count);

            for (size_t index = 0; index < count; index++)
            {
                fill.FillPtr(varXString[index], pointerByteCount * index);
                m_stream.AddPointerLookup(&varXString[index], fill.BlockBuffer(pointerByteCount * index));
            }
        }
    }
#endif

    for (size_t index = 0; index < count; index++)
    {
        LoadXString(false);
        varXString++;
    }
}

ZonePointerType ContentLoaderBase::GetZonePointerType(const void* zonePtr) const
{
    if (zonePtr == m_zone_ptr_following)
        return ZonePointerType::FOLLOWING;
    if (zonePtr == m_zone_ptr_insert)
        return ZonePointerType::INSERT;

    return ZonePointerType::OFFSET;
}
