#pragma once

#include "Asset/AssetCreationContext.h"
#include "Game/IW3/IW3.h"
#include "Parsing/Menu/Domain/CommonMenuDef.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW3
{
    class IMenuConverter
    {
    public:
        IMenuConverter() = default;
        virtual ~IMenuConverter() = default;

        virtual bool ConvertMenu(const menu::CommonMenuDef& commonMenu, menuDef_t& menu, AssetRegistration<AssetMenu>& registration) = 0;

        static std::unique_ptr<IMenuConverter> Create(bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context);
    };
} // namespace IW3
