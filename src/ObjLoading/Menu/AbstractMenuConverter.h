#pragma once

#include "Asset/AssetCreationContext.h"
#include "MenuConversionException.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

namespace menu
{
    class AbstractMenuConverter
    {
    protected:
        AbstractMenuConverter(bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context);

        _NODISCARD const char* ConvertString(const std::string& str) const;
        static void PrintConversionExceptionDetails(const MenuConversionException& e);

        bool m_disable_optimizations;
        ISearchPath& m_search_path;
        MemoryManager& m_memory;
        AssetCreationContext& m_context;
    };
} // namespace menu
