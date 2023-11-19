#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "MenuConversionException.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

namespace menu
{
    class AbstractMenuConverter
    {
    protected:
        bool m_disable_optimizations;
        ISearchPath* m_search_path;
        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;

        AbstractMenuConverter(bool disableOptimizations, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager);

        _NODISCARD const char* ConvertString(const std::string& str) const;

    public:
        static void PrintConversionExceptionDetails(const MenuConversionException& e);
    };
} // namespace menu
