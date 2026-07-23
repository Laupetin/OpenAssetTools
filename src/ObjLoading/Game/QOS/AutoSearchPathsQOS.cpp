#include "AutoSearchPathsQOS.h"

const std::vector<std::string>& AutoSearchPathsQOS::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "zone",
        "zone/english",
        "zone/french",
        "zone/german",
        "zone/italian",
        "zone/spanish",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsQOS::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {
        "main",
    };
    return additionalSearchPaths;
}
