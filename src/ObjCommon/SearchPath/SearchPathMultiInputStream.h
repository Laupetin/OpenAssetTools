#pragma once

#include "Parsing/Impl/ParserMultiInputStream.h"
#include "SearchPath/ISearchPath.h"

class SearchPathMultiInputStream : public IInclusionCallback
{
public:
    explicit SearchPathMultiInputStream(ISearchPath& searchPath);

    std::unique_ptr<std::istream> OpenIncludedFile(const std::string& filename, const std::string& sourceFile) override;

protected:
    ISearchPath& m_search_path;
};
