#include "AutoSearchPathsIW4.h"

const std::vector<std::string>& AutoSearchPathsIW4::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "zone/english",
        "zone/french",
        "zone/german",
        "zone/italian",
        "zone/spanish",
        "zone/british",
        "zone/russian",
        "zone/polish",
        "zone/korean",
        "zone/taiwanese",
        "zone/japanese",
        "zone/chinese",
        "zone/thai",
        "zone/leet",
        "zone/czech",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsIW4::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {
        "main",
    };
    return additionalSearchPaths;
}
