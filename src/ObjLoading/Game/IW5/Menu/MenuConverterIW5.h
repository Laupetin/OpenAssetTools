#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW5/IW5.h"
#include "Parsing/Menu/Domain/CommonMenuDef.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

namespace IW5
{
    class MenuConverter
    {
        bool m_disable_optimizations;
        ISearchPath* m_search_path;
        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;
        std::vector<XAssetInfoGeneric*> m_dependencies;

    public:
        MenuConverter(bool disableOptimizations, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager);

        std::vector<XAssetInfoGeneric*>& GetDependencies();
        _NODISCARD menuDef_t* ConvertMenu(const menu::CommonMenuDef& commonMenu);
    };
} // namespace IW5
