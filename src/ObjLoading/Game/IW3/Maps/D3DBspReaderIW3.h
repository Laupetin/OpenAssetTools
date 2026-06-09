#pragma once

#include "Game/IW3/Maps/D3DBspCommonIW3.h"
#include "SearchPath/ISearchPath.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace IW3::d3dbsp
{
    enum class LoadStatus
    {
        Loaded,
        NotFound,
        Invalid,
    };

    struct Lump
    {
        LumpType id;
        std::vector<std::byte> data;
    };

    class File
    {
    public:
        explicit File(std::string fileName);

        [[nodiscard]] const Lump* GetLump(LumpType id) const;

        std::string m_file_name;
        std::vector<Lump> m_lumps;
    };

    struct LoadResult
    {
        LoadStatus status = LoadStatus::NotFound;
        std::unique_ptr<File> file;
        std::string message;
    };

    [[nodiscard]] LoadResult LoadFromSearchPath(const std::string& assetName, ISearchPath& searchPath);
} // namespace IW3::d3dbsp
