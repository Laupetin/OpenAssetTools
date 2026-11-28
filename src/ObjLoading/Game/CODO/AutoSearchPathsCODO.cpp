#include "AutoSearchPathsCODO.h"

const std::vector<std::string>& AutoSearchPathsCODO::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "client_shipretail/chinese",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsCODO::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {};
    return additionalSearchPaths;
}
