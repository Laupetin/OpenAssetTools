#pragma once

#include "Asset/AssetCreationContext.h"
#include "Game/IW6/IW6.h"
#include "Parsing/Menu/Domain/CommonMenuDef.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace IW6
{
    class IMenuConverter
    {
    public:
        IMenuConverter() = default;
        virtual ~IMenuConverter() = default;

        virtual void ConvertMenu(const ::menu::CommonMenuDef& commonMenu, menuDef_t& menu, AssetRegistration<AssetMenu>& registration) = 0;

        static std::unique_ptr<IMenuConverter> Create(bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context);
    };
} // namespace IW6
