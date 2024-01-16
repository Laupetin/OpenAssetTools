#pragma once

#include "Cache/AbstractCacheReader.h"
#include "Game/IW4/IW4.h"
#include "Utils/ClassUtils.h"

namespace IW4
{
    class MenuCacheReader final : public cache::AbstractCacheReader
    {
    public:
        MenuCacheReader(std::istream& stream, MemoryManager& memory, ExpressionSupportingData* supportingData);

        _NODISCARD Statement_s* ReadStatement() const;
        _NODISCARD ItemKeyHandler* ReadItemKeyHandler() const;
        _NODISCARD MenuEventHandler* ReadMenuEventHandler() const;
        _NODISCARD MenuEventHandlerSet* ReadMenuEventHandlerSet() const;

        void ReadWindowDef(windowDef_t& windowDef) const;

        _NODISCARD itemDef_s* ReadItem() const;
        _NODISCARD menuDef_t* ReadMenu() const;

        ExpressionSupportingData* m_supporting_data;
    };
} // namespace IW4
