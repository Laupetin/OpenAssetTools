#include "ObjLoading.h"

#include "IObjLoader.h"
#include "ObjContainer/IWD/IWD.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/ObjFileStream.h"

#include <fstream>

ObjLoading::Configuration_t ObjLoading::Configuration;

void ObjLoading::LoadIWDsInSearchPath(ISearchPath& searchPath)
{
    searchPath.Find(SearchPathSearchOptions().IncludeSubdirectories(false).FilterExtensions("iwd"),
                    [&searchPath](const std::string& path)
                    {
                        auto file = std::make_unique<std::ifstream>(path, std::fstream::in | std::fstream::binary);

                        if (file->is_open())
                        {
                            auto iwd = std::make_unique<IWD>(path, std::move(file));

                            if (iwd->Initialize())
                            {
                                IWD::Repository.AddContainer(std::move(iwd), &searchPath);
                            }
                        }
                    });
}

void ObjLoading::UnloadIWDsInSearchPath(ISearchPath& searchPath)
{
    IWD::Repository.RemoveContainerReferences(&searchPath);
}

SearchPaths ObjLoading::GetIWDSearchPaths()
{
    SearchPaths iwdPaths;

    for (auto* iwd : IWD::Repository)
    {
        iwdPaths.IncludeSearchPath(iwd);
    }

    return iwdPaths;
}
