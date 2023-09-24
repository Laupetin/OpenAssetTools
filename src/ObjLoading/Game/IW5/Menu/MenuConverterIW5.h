#pragma once

#include "Utils/ClassUtils.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW5/IW5.h"
#include "Parsing/Menu/Domain/CommonMenuDef.h"
#include "Utils/MemoryManager.h"
#include "SearchPath/ISearchPath.h"

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
}
