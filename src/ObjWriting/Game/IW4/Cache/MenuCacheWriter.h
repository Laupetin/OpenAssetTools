#pragma once

#include "Cache/AbstractCacheWriter.h"
#include "Game/IW4/IW4.h"

#include <ostream>

namespace IW4
{
    class MenuCacheWriter : public cache::AbstractCacheWriter
    {
    public:
        explicit MenuCacheWriter(std::ostream& stream);

        void WriteStatement(const Statement_s& statement) const;
        void WriteItemKeyHandler(const ItemKeyHandler& itemKeyHandler) const;
        void WriteMenuEventHandler(const MenuEventHandler& menuEventHandler) const;
        void WriteMenuEventHandlerSet(const MenuEventHandlerSet& menuEventHandlerSet) const;

        void WriteWindowDef(const windowDef_t& windowDef) const;

        void WriteItem(const itemDef_s& item) const;
        void WriteMenu(const menuDef_t& menu) const;
    };
} // namespace IW4
